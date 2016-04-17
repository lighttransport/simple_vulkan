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
			vk::PhysicalDevice physicalDevice,
            vk::Device device,
            vk::SurfaceKHR& surface,
            vk::ImageUsageFlags usage,
            uint32_t width,
            uint32_t height)
    {
        m_device = device;
        m_usage = usage;
        m_width = width;
        m_height = height;

		vk::SurfaceTransformFlagBitsKHR transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
		vk::CompositeAlphaFlagBitsKHR compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

        vk::Result result;

		//get SurfaceFormats
		uint32_t surfaceFormatsCount;
		result = physicalDevice.getSurfaceFormatsKHR(surface, &surfaceFormatsCount, nullptr);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }
		std::vector<vk::SurfaceFormatKHR> surfaceFormats(surfaceFormatsCount);
		result = physicalDevice.getSurfaceFormatsKHR(surface, &surfaceFormatsCount, surfaceFormats.data());
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

		m_format = surfaceFormats[0].format();

		//get SurfacePresentModes
		uint32_t surfacePresentModesCount;
		result = physicalDevice.getSurfacePresentModesKHR(surface, &surfacePresentModesCount, nullptr);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }
		std::vector<vk::PresentModeKHR> surfacePresentModes(surfacePresentModesCount);
		result = physicalDevice.getSurfacePresentModesKHR(surface, &surfacePresentModesCount, surfacePresentModes.data());
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

		//get SurfaceCapablilies
		vk::SurfaceCapabilitiesKHR surfaceCapabilities;
		result = physicalDevice.getSurfaceCapabilitiesKHR(surface, &surfaceCapabilities);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

		if (!(surfaceCapabilities.supportedUsageFlags() & usage))
		{
			return vk::Result::eErrorInitializationFailed;
		}
		if (!(surfaceCapabilities.supportedTransforms() & transform))
		{
			return vk::Result::eErrorInitializationFailed;
		}
		if (!(surfaceCapabilities.supportedCompositeAlpha() & compositeAlpha))
		{
			return vk::Result::eErrorInitializationFailed;
		}


        //init SwapchainCreateInfo
        vk::SwapchainCreateInfoKHR swapchainInfo;
        swapchainInfo.flags(vk::SwapchainCreateFlagBitsKHR{});
        swapchainInfo.surface(surface);
        swapchainInfo.minImageCount(surfaceCapabilities.minImageCount());
        swapchainInfo.imageFormat(surfaceFormats[0].format());
        swapchainInfo.imageColorSpace(surfaceFormats[0].colorSpace());
        swapchainInfo.imageExtent(vk::Extent2D(width,height));
        swapchainInfo.imageArrayLayers(1);
        swapchainInfo.imageUsage(m_usage);
        swapchainInfo.imageSharingMode(vk::SharingMode::eExclusive);
        swapchainInfo.queueFamilyIndexCount(0);
        swapchainInfo.pQueueFamilyIndices(nullptr);
        swapchainInfo.preTransform(transform);
        swapchainInfo.compositeAlpha(compositeAlpha);
        swapchainInfo.presentMode(surfacePresentModes[0]);
        swapchainInfo.clipped(true);
        swapchainInfo.oldSwapchain(vk::SwapchainKHR());

        //create Swapchain
        result = m_device.createSwapchainKHR(&swapchainInfo,nullptr,&m_swapchain);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

        //get SwapchainImages
        uint32_t swapchainCount;
        result = m_device.getSwapchainImagesKHR(m_swapchain,&swapchainCount,nullptr);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }
        m_images.resize(swapchainCount);
        result = m_device.getSwapchainImagesKHR(m_swapchain,&swapchainCount,m_images.data());
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

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
