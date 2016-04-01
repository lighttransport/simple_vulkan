#pragma once

#include<vector>
#include<vulkan/vk_cpp.h>

#include"instance.h"

namespace simpleVulkan
{
    class Device
    { 
   private:
       vk::Device m_device;
   public:
       Device();
       ~Device();

       Result create(vk::Instance instance);
       void destroy();

       vk::Device& getVkDevice();
    };
}
