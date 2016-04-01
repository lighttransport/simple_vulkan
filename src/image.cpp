#include<iostream>
#include<image.h>

namespace simpleVulkan
{
    Image::Image()
    {}

    Image::~Image(){}

    Result Image::create(
            vk::Device device,
            vk::Format format,
            vk::ImageUsageFlags usage,
            uint32_t width,
            uint32_t height)
    {
        m_device = device;
        m_format = format;
        m_usage = usage;
        m_width = width;
        m_height = height;

        vk::Result result;

        //init ImageCreateInfo
        vk::ImageCreateInfo imageInfo;
        imageInfo.imageType(vk::ImageType::e2D);
        imageInfo.format(m_format);
        imageInfo.extent().width(m_width);
        imageInfo.extent().height(m_height);
        imageInfo.extent().depth(1);
        imageInfo.mipLevels(1);
        imageInfo.arrayLayers(1);
        imageInfo.samples(vk::SampleCountFlagBits::e1);
        imageInfo.tiling(vk::ImageTiling::eLinear); //maybe
        imageInfo.usage(m_usage);
        imageInfo.sharingMode(vk::SharingMode::eExclusive);
        imageInfo.queueFamilyIndexCount(0);
        imageInfo.pQueueFamilyIndices(nullptr);
        imageInfo.initialLayout(vk::ImageLayout::eUndefined);

        //create DepthImage
        result = m_device.createImage(&imageInfo,nullptr,&m_image); 

        //init MemoryAlloateInfo
        vk::MemoryRequirements req;
        m_device.getImageMemoryRequirements(m_image,&req);
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.allocationSize(req.size());
        allocInfo.memoryTypeIndex(1); //maybe
        
        //alloc DepthMemory
        result = m_device.allocateMemory(&allocInfo,nullptr,&m_deviceMemory);
    
        //bind Image
        m_device.bindImageMemory(m_image,m_deviceMemory,0);

        vk::ImageAspectFlags aspect;
        if(m_usage & vk::ImageUsageFlagBits::eColorAttachment)
        {
            aspect |= vk::ImageAspectFlagBits::eColor;
        }
        if(m_usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
        {
            aspect |= vk::ImageAspectFlagBits::eDepth;
        }

        //init ImageViewCreateInfo
        vk::ImageViewCreateInfo imageViewInfo;
        imageViewInfo.flags(vk::ImageViewCreateFlagBits());
        imageViewInfo.image(m_image);
        imageViewInfo.format(m_format);
        imageViewInfo.components().r(vk::ComponentSwizzle::eR);
        imageViewInfo.components().g(vk::ComponentSwizzle::eG);
        imageViewInfo.components().b(vk::ComponentSwizzle::eB);
        imageViewInfo.components().a(vk::ComponentSwizzle::eA);
        imageViewInfo.subresourceRange().aspectMask(aspect);
        imageViewInfo.subresourceRange().baseMipLevel(0);
        imageViewInfo.subresourceRange().levelCount(1);
        imageViewInfo.subresourceRange().baseArrayLayer(0);
        imageViewInfo.subresourceRange().layerCount(1);
        imageViewInfo.viewType(vk::ImageViewType::e2D);
       
        //create ImageView
        result = m_device.createImageView(&imageViewInfo,nullptr,&m_imageView);
        return result;
    }

    void Image::destroy()
    {
        m_device.destroyImageView(m_imageView,nullptr);
        m_device.freeMemory(m_deviceMemory,nullptr);
        m_device.destroyImage(m_image,nullptr);
    }

    vk::Image& Image::getVkImage()
    {
        return m_image;
    }
    vk::ImageView& Image::getVkImageView()
    {
        return m_imageView;
    }
    int32_t Image::getWidth()
    {
        return m_width;
    }
    int32_t Image::getHeight()
    {
        return m_height;
    }
}
