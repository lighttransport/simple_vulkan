#include"render_pass.h"

namespace simpleVulkan
{
    RenderPass::RenderPass()
    {
    }
    RenderPass::~RenderPass()
    {
    }

    Result RenderPass::create(vk::Device device,vk::Format colorFormat,vk::Format depthFormat)
    {
        m_device = device;

        vk::Result result;
        //init AttachmentDescription
        vk::AttachmentDescription attachments[2];
        attachments[0].format(colorFormat);
        attachments[0].samples(vk::SampleCountFlagBits::e1);
        attachments[0].loadOp(vk::AttachmentLoadOp::eClear);
        attachments[0].storeOp(vk::AttachmentStoreOp::eStore);
        attachments[0].stencilLoadOp(vk::AttachmentLoadOp::eDontCare);
        attachments[0].stencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        attachments[0].initialLayout(vk::ImageLayout::eColorAttachmentOptimal);
        attachments[0].finalLayout(vk::ImageLayout::eColorAttachmentOptimal);
        attachments[0].flags(vk::AttachmentDescriptionFlagBits());
        attachments[1].format(depthFormat);
        attachments[1].samples(vk::SampleCountFlagBits::e1);
        attachments[1].loadOp(vk::AttachmentLoadOp::eClear);
        attachments[1].storeOp(vk::AttachmentStoreOp::eDontCare);
        attachments[1].stencilLoadOp(vk::AttachmentLoadOp::eDontCare);
        attachments[1].stencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        attachments[1].initialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
        attachments[1].finalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
        attachments[1].flags(vk::AttachmentDescriptionFlagBits());

        //init AttachmentReference
        vk::AttachmentReference colorRef;
        colorRef.attachment(0);
        colorRef.layout(vk::ImageLayout::eColorAttachmentOptimal);
        vk::AttachmentReference depthRef;
        depthRef.attachment(1);
        depthRef.layout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
  
        //init SubpassDescription
        vk::SubpassDescription subpassDesc;
        subpassDesc.flags(vk::SubpassDescriptionFlagBits());
        subpassDesc.inputAttachmentCount(0);
        subpassDesc.pInputAttachments(nullptr);
        subpassDesc.colorAttachmentCount(1);
        subpassDesc.pColorAttachments(&colorRef);
        subpassDesc.pResolveAttachments(nullptr);
        subpassDesc.pDepthStencilAttachment(&depthRef);
        subpassDesc.pDepthStencilAttachment(nullptr);
        subpassDesc.preserveAttachmentCount(0);
        subpassDesc.pPreserveAttachments(nullptr);

        //init RenderPassCreateInfo
        vk::RenderPassCreateInfo renderPassInfo;
        renderPassInfo.flags(vk::RenderPassCreateFlagBits());
        renderPassInfo.attachmentCount(2);
        renderPassInfo.pAttachments(attachments);
        renderPassInfo.subpassCount(1);
        renderPassInfo.pSubpasses(&subpassDesc);
        renderPassInfo.dependencyCount(0);
        renderPassInfo.pDependencies(nullptr);

        //create RenderPass
        result = m_device.createRenderPass(&renderPassInfo,nullptr,&m_renderPass);
        return result;
    }

    void RenderPass::destroy()
    {
        m_device.destroyRenderPass(m_renderPass,nullptr);
    }

    vk::RenderPass& RenderPass::getVkRenderPass()
    {
        return m_renderPass;
    }
}
