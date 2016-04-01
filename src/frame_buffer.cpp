#include"frame_buffer.h"
#include"image.h"
#include"render_pass.h"

namespace simpleVulkan
{
    Framebuffer::Framebuffer()
    {
    }

    Framebuffer::~Framebuffer()
    {
    }
       
    Result Framebuffer::create(
                vk::Device device,
                uint32_t width,
                uint32_t height,
                vk::ImageView colorImageView,
                vk::ImageView depthImageView,
                vk::RenderPass renderPass)
    {
        m_device = device;
        vk::Result result;
        vk::ImageView attachments[2];
        attachments[0] = colorImageView;
        attachments[1] = depthImageView;

        //init FramebufferCreateInfo
        vk::FramebufferCreateInfo framebufferInfo;
        framebufferInfo.flags(vk::FramebufferCreateFlagBits());
        framebufferInfo.renderPass(renderPass);
        framebufferInfo.attachmentCount(2);
        framebufferInfo.pAttachments(attachments);
        framebufferInfo.width(width);
        framebufferInfo.height(height);
        framebufferInfo.layers(1);

        //create Framebuffer
        result = m_device.createFramebuffer(&framebufferInfo,nullptr,&m_framebuffer);
        return result;
    }

    void Framebuffer::destroy()
    {
        m_device.destroyFramebuffer(m_framebuffer,nullptr);
    }

    vk::Framebuffer& Framebuffer::getVkFrameBuffer()
    {
        return m_framebuffer;
    }
}
