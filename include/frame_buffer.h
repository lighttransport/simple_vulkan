#pragma once

#include"device.h"
#include"image.h"
#include"render_pass.h"

namespace simpleVulkan
{
    class Framebuffer
    {
        vk::Device m_device;
        vk::Framebuffer m_framebuffer;
   public:
        Framebuffer();
        ~Framebuffer();

        virtual Result create(
                vk::Device device,
                uint32_t width,
                uint32_t height,
                vk::ImageView colorImageView,
                vk::ImageView depthImageView,
                vk::RenderPass renderPass);
        virtual void destroy();
       
        vk::Framebuffer& getVkFrameBuffer();
    };
}
