#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vector>
#include <iostream>

#include "VulkanRenderer.h"

VulkanRenderer vulkanrenderer;
GLFWwindow *window;

void initWindow(std::string wName = "Test Window", const int width = 800, const int height = 600)
{
	// Initialise GLFW
	glfwInit();
	// Set GLFW to NOT work with OpenGL
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, wName.c_str(),  nullptr, nullptr);
}

int main()
{
	//Create Window
	initWindow("Test Window", 800, 600);

	//create Vulkan Renderer instance
	if (vulkanrenderer.init(window) == EXIT_FAILURE) {
		return EXIT_FAILURE;
	}
	vulkanrenderer.init(window);


	//Loop until closed
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}
	
	//Destroying the instance
	vulkanrenderer.cleanup();


	//Destro GLFW window and stop GLFW
	glfwDestroyWindow(window);
	glfwTerminate();


	return 0;
}