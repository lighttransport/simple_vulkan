#pragma once
#include<vector>
#include<vulkan/vk_cpp.hpp>

#include"device.h"

namespace simpleVulkan
{
    class CommandBuffers
    {
   private:
       vk::Device m_device;
       vk::CommandPool m_cmdPool;
       std::vector<vk::CommandBuffer> m_cmdBuffers;
   public:
       CommandBuffers();
       ~CommandBuffers();

       Result create(vk::Device device,size_t count);
       void destroy();

       vk::CommandPool& getVkCommandPool();
       vk::CommandBuffer& getVkCommandBuffer(size_t index);
       const std::vector<vk::CommandBuffer>& getVkCommandBuffers();
    };
}
