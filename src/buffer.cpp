#include"buffer.h"

namespace simpleVulkan
{

    Buffer::Buffer()
    {
    }
    Buffer::~Buffer()
    {
    }
    Result Buffer::create(vk::Device device,vk::BufferUsageFlags usage,size_t size)
    {
        m_device = device;
        m_usage = usage;
        m_size = size;

        vk::Result result;

        //init BufferCreateInfo
        vk::BufferCreateInfo bufferInfo;
        bufferInfo.flags(vk::BufferCreateFlagBits());
        bufferInfo.usage(m_usage);
        bufferInfo.size(m_size);
        bufferInfo.queueFamilyIndexCount(0);
        bufferInfo.pQueueFamilyIndices(nullptr);
        bufferInfo.sharingMode(vk::SharingMode::eExclusive);

        //create VertexBuffer
        result = m_device.createBuffer(&bufferInfo,nullptr,&m_buffer);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

        //init MemoryAllocateInfo
        vk::MemoryRequirements req;
        m_device.getBufferMemoryRequirements(m_buffer,&req);
        vk::MemoryAllocateInfo memoryInfo;
        memoryInfo.memoryTypeIndex(0);
        memoryInfo.allocationSize(req.size());

        //allocate VertexMemory
        result = m_device.allocateMemory(&memoryInfo,nullptr,&m_memory);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

        //bind VertexMemory to VertexBuffer
        result = m_device.bindBufferMemory(m_buffer,m_memory,0);
        return result;
    }

    void Buffer::destroy()
    {
        m_device.freeMemory(m_memory,nullptr);
        m_device.destroyBuffer(m_buffer,nullptr);
    }
    
    void Buffer::write(const void* pData)
    {
        vk::Result result;

        void* mappedMemory = nullptr;

        //map VertexMemory
        result = m_device.mapMemory(m_memory,0,m_size,vk::MemoryMapFlagBits(),&mappedMemory);

        //copy Vertexes
        memcpy(mappedMemory,pData,m_size);

        //unmap VertexMemory
        m_device.unmapMemory(m_memory);

    }

    size_t Buffer::getSize()
    {
        return m_size;
    }
   
    vk::Buffer& Buffer::getVkBuffer()
    {
        return m_buffer;
    }
}
