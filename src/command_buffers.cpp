#include"command_buffers.h"

#include<device.h>

namespace simpleVulkan
{
    CommandBuffers::CommandBuffers()
    {
    }

    CommandBuffers::~CommandBuffers(){}

    Result CommandBuffers::create(vk::Device device,size_t count)
    {
        vk::Result result;
        m_device = device;
        //init CommandPoolCreateInfo
        vk::CommandPoolCreateInfo cmdPoolInfo;
        cmdPoolInfo.queueFamilyIndex(0);
        cmdPoolInfo.flags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        //create CommandPool
        result = m_device.createCommandPool(&cmdPoolInfo,nullptr,&m_cmdPool);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

        //init CommandBufferAllocateInfo
        vk::CommandBufferAllocateInfo cmdBufAllocateInfo;
        cmdBufAllocateInfo.commandPool(m_cmdPool);
        cmdBufAllocateInfo.level(vk::CommandBufferLevel::ePrimary);
        cmdBufAllocateInfo.commandBufferCount(count);

        m_cmdBuffers.resize(count);
        //allocate CommandBuffer
        result = m_device.allocateCommandBuffers(&cmdBufAllocateInfo,m_cmdBuffers.data());
        return result;
    }

    void CommandBuffers::destroy()
    {
        m_device.destroyCommandPool(m_cmdPool,nullptr);;
    }

    vk::CommandPool& CommandBuffers::getVkCommandPool()
    {
        return m_cmdPool;
    }
    vk::CommandBuffer& CommandBuffers::getVkCommandBuffer(size_t index)
    {
        return m_cmdBuffers[index];
    }
    const std::vector<vk::CommandBuffer>& CommandBuffers::getVkCommandBuffers()
    {
        return m_cmdBuffers;
    }

}
