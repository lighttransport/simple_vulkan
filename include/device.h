#pragma once

#include<vector>
#include<vulkan/vk_cpp.h>

#include"instance.h"

namespace simpleVulkan
{
    class Device
    { 
   private:
	   vk::PhysicalDevice m_physicalDevice;
       vk::Device m_device;
   public:
       Device();
       ~Device();

       Result create(vk::Instance instance);
       void destroy();

	   vk::PhysicalDevice& getVkPhysicalDevice();
       vk::Device& getVkDevice();
    };
}
