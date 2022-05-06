#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer()
{
}

int VulkanRenderer::init(GLFWwindow * newWindow)
{
	window = newWindow;
	try {
		createInstance();
		getPhysicalDevice();
		createlogicaldevice();
	}
	catch (const std::runtime_error &e) {
		printf("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}

	return 0;
}

void VulkanRenderer::cleanup()
{
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	vkDestroyInstance(instance, nullptr);
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::createInstance()
{
	//Information about the appplication itself
	//This data doesnt effect the program, it is just for developer  convinience
	VkApplicationInfo appinfo{};
	appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appinfo.pApplicationName = "Vulkan App";
	appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appinfo.pEngineName = "No Engine";
	appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appinfo.apiVersion = VK_API_VERSION_1_0;



	//Creation information for a vulkan instance  
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appinfo;

	//create list to hold instance extensions
	std::vector<const char*> instanceExtensions = std::vector<const char*>();

	//setup extensions this instance will use
	uint32_t glfwExtensionCount = 0;    //GLFW might require multiple extensions
	const char** glfwExtensions;        //passed as an array of cstrings , so need pointer (the array ) to pointer (the cstring)
	//Get vulkan instance extensions required by GLFW
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	//Add GLFW extensions to list of extensions
	for (size_t i = 0; i < glfwExtensionCount; i++) {
		instanceExtensions.push_back(glfwExtensions[i]);
	}

	//Check Instance extensions supported
	if (!checkInstanceExtensionSupport(&instanceExtensions))
	{
		throw std::runtime_error("Vkinstance does not support required extensions");
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t> (instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();
	//TODO : Set up validation layers for this instance
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
	
	//create Instance
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if(result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Instance");
	}



}

void VulkanRenderer::createlogicaldevice()
{  
	//Get the queue family indices for the chosen Physical device
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	//qQueue the logical device needs to create and info to do s0 (only 1 for now, will add more later
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;     //The Index of the family to create a queue from it
	queueCreateInfo.queueCount = 1;    //Number of queues to create
	float priority = 1.0f;
	queueCreateInfo.pQueuePriorities = &priority;                //Vulkan needs to know how to handle multiple queues, so decide priority 1 = highest priority

	//Information to create logical device
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;                  //Number of Queue create infos
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;     //List of queue create infos so device can create required queues
	deviceCreateInfo.enabledExtensionCount = 0;
	deviceCreateInfo.ppEnabledExtensionNames = nullptr;
	
	//Physical device features the logical device will be using
	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;       //Physical Device features, logical device will be using

	//Create the logic device for the given physical device
	VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create a logical device");
	}

	//Queues are created at the same time as the device..
	//So we want to handle queues

	vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
}

void VulkanRenderer::getPhysicalDevice()
{
	//Enumerate Physical devices the vkInstance can access
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	//If no devices available,then none support Vulkan

	if (deviceCount == 0) {
		throw std::runtime_error("Cant find GPUs that support Vulkan Instance");
	}

	std::vector<VkPhysicalDevice> deviceList(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

	//TEMP: Just pick first device

	for (const auto &device : deviceList) 
	{
		if (checkDeviceSuitable(device)) {
			mainDevice.physicalDevice = device;
			break;
		}
	}
}

bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions)
{   //Need to get number of extensions to create array of correct size to hold extensions

	uint32_t extensioncount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensioncount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensioncount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensioncount, extensions.data());


	//Check if given extensions are in te list of available extensions
	for (const auto &checkExtension : *checkExtensions)
	{
		bool hasExtension = false;
		for (const auto &extension : extensions) {
			if (strcmp(checkExtension, extension.extensionName))
			{
				hasExtension = true;
				break;
			}
		}
		if (!hasExtension)
		{
			return false;
		}
	}

	return true;
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
	/*
	//Information about the device itself(ID,name,type,vencor,etc)
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	//Information about what the device can do (geo shader, tess shader, widelines,etc)
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	*/
	QueueFamilyIndices indices = getQueueFamilies(device);
	return indices.isValid();
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	
	QueueFamilyIndices indices;
	uint32_t queuefamilycount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queuefamilycount, nullptr);
	std::vector <VkQueueFamilyProperties> queueFamilyList(queuefamilycount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queuefamilycount, queueFamilyList.data());

	//Go through each queue family and check if it has at least 1 of the required type of queue
	//Queue can be 
	int i = 0;
	for (const auto &queueFamily : queueFamilyList)
	{
		//First check if queue family has atleast one queue in that family
		//Queue can be multiples types defined through bitfield.Need to bitwise AND with VK_QUEUE_*_BIT to check if has required type
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}
		if (indices.isValid())
		{
			break;
		}
		i++;
	}
	return indices;
}
