#include"buffer.h"

namespace simpleVulkan
{

    Buffer::Buffer()
    {
    }
    Buffer::~Buffer()
    {
    }
	Result Buffer::create(vk::PhysicalDevice physicalDevice,vk::Device device, vk::BufferUsageFlags usage, size_t size)
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
		result = m_device.createBuffer(&bufferInfo, nullptr, &m_buffer);
		if (result != vk::Result::eSuccess)
		{
			return result;
		}

		//get PhysicalDeviceMemoryProperties
		vk::PhysicalDeviceMemoryProperties memoryProperties;
		physicalDevice.getMemoryProperties(&memoryProperties);

		//get MemoryRequirements
		vk::MemoryRequirements req;
		m_device.getBufferMemoryRequirements(m_buffer, &req);

		uint32_t memoryTypeBits = req.memoryTypeBits();
		uint32_t memoryTypeIndex = 0;
		for (int i = 0; i < (sizeof(memoryTypeBits) * 8); ++i)
		{
			if ((memoryTypeBits >> i) & 1)
			{
				std::cout << (uint32_t)memoryProperties.memoryTypes()[i].propertyFlags() << std::endl;
				if(memoryProperties.memoryTypes()[i].propertyFlags() & vk::MemoryPropertyFlagBits::eHostVisible)
				{
					memoryTypeIndex = i;
					break;
				}
			}
		}

        //init MemoryAllocateInfo
        vk::MemoryAllocateInfo memoryInfo;
        memoryInfo.memoryTypeIndex(memoryTypeIndex);
        memoryInfo.allocationSize(req.size());

        //allocate VertexMemory
        result = m_device.allocateMemory(&memoryInfo,nullptr,&m_memory);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

        //bind VertexMemory to VertexBuffer
#ifdef VKCPP_ENHANCED_MODE
        m_device.bindBufferMemory(m_buffer,m_memory,0);
#else
        result = m_device.bindBufferMemory(m_buffer,m_memory,0);
#endif
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

#ifdef VKCPP_ENHANCED_MODE
		mappedMemory = m_device.mapMemory(m_memory,0,m_size,vk::MemoryMapFlagBits());
#else
		result = m_device.mapMemory(m_memory,0,m_size,vk::MemoryMapFlagBits(),&mappedMemory);
#endif
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
