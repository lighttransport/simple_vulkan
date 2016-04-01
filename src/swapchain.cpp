#include"swapchain.h"

namespace simpleVulkan
{
    Swapchain::Swapchain()
    {
    }
    Swapchain::~Swapchain()
    {
    }
    Result Swapchain::create(
            vk::Device device,
            vk::SurfaceKHR& surface,
            vk::ImageUsageFlags usage,
            vk::Format format,
            uint32_t width,
            uint32_t height)
    {
        m_device = device;
        m_format = format;
        m_usage = usage;
        m_width = width;
        m_height = height;

        vk::Result result;

        //init SwapchainCreateInfo
        vk::SwapchainCreateInfoKHR swapchainInfo;
        swapchainInfo.flags(vk::SwapchainCreateFlagBitsKHR{});
        swapchainInfo.surface(surface);
        swapchainInfo.minImageCount(1); //3 //debug
        swapchainInfo.imageFormat(format);
        swapchainInfo.imageColorSpace(vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinearKHR);
        swapchainInfo.imageExtent(vk::Extent2D(width,height));
        swapchainInfo.imageArrayLayers(1);
        swapchainInfo.imageUsage(m_usage);
        swapchainInfo.imageSharingMode(vk::SharingMode::eExclusive);
        swapchainInfo.queueFamilyIndexCount(0);
        swapchainInfo.pQueueFamilyIndices(nullptr);
        swapchainInfo.preTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
        swapchainInfo.compositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        swapchainInfo.presentMode(vk::PresentModeKHR::eFifoKHR);
        swapchainInfo.clipped(true);
        swapchainInfo.oldSwapchain(nullptr);

        //create Swapchain
        m_device.createSwapchainKHR(&swapchainInfo,nullptr,&m_swapchain);

        //get SwapchainImages
        uint32_t swapchainCount;
        result = m_device.getSwapchainImagesKHR(m_swapchain,&swapchainCount,nullptr);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }
        m_images.resize(swapchainCount);
        m_device.getSwapchainImagesKHR(m_swapchain,&swapchainCount,m_images.data());

        //create SwapchainImageViews
        m_imageViews.resize(swapchainCount);
        vk::ImageAspectFlags aspect;
        if(m_usage & vk::ImageUsageFlagBits::eColorAttachment)
        {
            aspect |= vk::ImageAspectFlagBits::eColor;
        }
        if(m_usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
        {
            aspect |= vk::ImageAspectFlagBits::eDepth;
        }
        for(uint32_t i=0;i<swapchainCount;++i)
        {
            //init SwapchainImageViewCreateInfo
            vk::ImageViewCreateInfo imageViewInfo;
            imageViewInfo.flags(vk::ImageViewCreateFlagBits());
            imageViewInfo.image(m_images[i]);
            imageViewInfo.viewType(vk::ImageViewType::e2D);
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

            //create SwapchainImageView
            result = m_device.createImageView(&imageViewInfo,nullptr,&m_imageViews[i]);
            if(result != vk::Result::eSuccess)
            {
                return result;
            }
        }
        return result;
    }

    vk::ImageUsageFlags Swapchain::getUsage()
    {
        return m_usage;
    }
    vk::Format Swapchain::getFormat()
    {
        return m_format;
    }
    uint32_t Swapchain::getWidth()
    {
        return m_width;
    }
    uint32_t Swapchain::getHeight()
    {
        return m_height;
    }
    size_t Swapchain::count()
    {
        return m_images.size();
    }
    vk::SwapchainKHR& Swapchain::getVkSwapchainKHR()
    {
        return m_swapchain;
    }
    vk::Image& Swapchain::getVkImage(size_t index)
    {
        return m_images[index];
    }
    vk::ImageView& Swapchain::getVkImageView(size_t index)
    {
        return m_imageViews[index];
    }

}
