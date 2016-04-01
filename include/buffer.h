#pragma once

#include"device.h"

namespace simpleVulkan
{
    class Buffer
    {
        vk::Device m_device;
        vk::BufferUsageFlags m_usage;
        size_t m_size;
        vk::Buffer m_buffer;
        vk::DeviceMemory m_memory;
   public:
        Buffer();
        ~Buffer();

        Result create(vk::Device device,vk::BufferUsageFlags usage,size_t size);
        void destroy();
        void write(const void* pData);

        size_t getSize();
        vk::BufferUsageFlags getUsage();

        vk::Buffer& getVkBuffer();
    };
}
