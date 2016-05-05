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
       Result submit(
		   vk::CommandBuffer cmdBuffer,
		   const std::vector<vk::Semaphore>& waitSemaphores = std::vector<vk::Semaphore>(),
		   const std::vector<vk::Semaphore>& signalSemaphores = std::vector<vk::Semaphore>());
       Result wait();
       Result present(vk::SwapchainKHR swapchain,uint32_t index);

       vk::Queue& getVkQueue();
   };
}
