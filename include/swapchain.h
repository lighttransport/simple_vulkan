#pragma once

#include<vulkan/vk_cpp.h>

#include<device.h>
#include<vector>

#include"image.h"

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
        std::vector<simpleVulkan::Image> m_images;
   public:
        Swapchain();
        ~Swapchain();

		Result create(
				vk::PhysicalDevice physicalDevice,
                vk::Device device,
                vk::SurfaceKHR surface,
                vk::ImageUsageFlags usage,
                uint32_t width,
                uint32_t height);
        void destroy();

        std::vector<simpleVulkan::Image>& getImages();
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
