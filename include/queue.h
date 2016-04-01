#pragma once

#include<vulkan/vk_cpp.h>

#include"device.h"

namespace simpleVulkan
{
    class Queue
    {
   private:
       vk::Device m_device;
       vk::Queue m_queue;
   public:
       Queue();
       ~Queue();

       void init(vk::Device deivce);

       vk::Queue& getVkQueue();
   };
}
