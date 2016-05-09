#pragma once

#include<vulkan/vk_cpp.hpp>

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
       Result submit(vk::CommandBuffer cmdBuffer);
       Result submit(vk::CommandBuffer cmdBuffer,vk::Semaphore semaphore);
       Result wait();
       Result present(vk::SwapchainKHR swapchain,uint32_t index);

       vk::Queue& getVkQueue();
   };
}
