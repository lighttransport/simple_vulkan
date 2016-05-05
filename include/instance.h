#pragma once


#include<string>
#include<vector>
#include<vulkan/vk_cpp.h>
#include<GLFW/glfw3.h>

#include<iostream>

#ifndef VK_API_VERSION
#define VK_API_VERSION VK_API_VERSION_1_0
#endif

namespace simpleVulkan
{
    using Result = vk::Result;
    class Instance
    {
       vk::Instance m_instance;
	   vk::SurfaceKHR m_surface;
   public:
       Instance();
       ~Instance();

       Result create(
               std::string applicationName,
               std::uint32_t applicationVersion,
               std::string engineName,
               std::uint32_t engineVersion,
               const std::vector<const char*>& extensions,
               const std::vector<const char*>& layers,
			   GLFWwindow* window);
       void destroy();

       vk::Instance getVkInstance();
	   vk::SurfaceKHR getSurface();
    };
}
