#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include "Utilities.h"
class VulkanRenderer 
{
public:
	VulkanRenderer();

	int init(GLFWwindow *newWindow);
	void cleanup();
	~VulkanRenderer();


private:
	GLFWwindow *window;

	//Vulkan Components
    VkInstance instance;

	struct   {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	}mainDevice;

	VkQueue graphicsQueue;

	//Vulkan Functions
	//Create functions
	void createInstance();
	void createlogicaldevice();
	// Get Functions for physical device
	void getPhysicalDevice();

	//Support Functions
	//Checker Functions
	bool checkInstanceExtensionSupport(std::vector<const char*> * checkExtensions);
	bool checkDeviceSuitable(VkPhysicalDevice device);

	//Getter functions
    QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);

};