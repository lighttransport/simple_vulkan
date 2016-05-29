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

       Result create(vk::Instance instance,size_t deviceIndex, bool validate);
       void destroy();
       size_t count();

	   vk::PhysicalDevice& getVkPhysicalDevice();
       vk::Device& getVkDevice();
    };
}
