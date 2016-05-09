#pragma once

#include<vulkan/vk_cpp.hpp>

#include<device.h>
#include<vector>

#include"device.h"

namespace simpleVulkan
{
    class Swapchain
    {
        vk::Device m_device;
        vk::ImageUsageFlags m_usage;
        vk::Format m_format;
        uint32_t m_width;
        uint32_t m_height;
        vk::SwapchainKHR m_swapchain;
        std::vector<vk::Image> m_images;
        std::vector<vk::ImageView> m_imageViews;
   public:
        Swapchain();
        ~Swapchain();

		Result create(
				vk::PhysicalDevice physicalDevice,
                vk::Device device,
                vk::SurfaceKHR& surface,
                vk::ImageUsageFlags usage,
                uint32_t width,
                uint32_t height);
        void destroy();

        vk::ImageUsageFlags getUsage();
        vk::Format getFormat();
        uint32_t getWidth();
        uint32_t getHeight();
        size_t count();

        vk::SwapchainKHR& getVkSwapchainKHR();
        vk::Image& getVkImage(size_t index);
        vk::ImageView& getVkImageView(size_t index);
    };
}
