#pragma once

#include"device.h"

namespace simpleVulkan
{
    class RenderPass
    {
        vk::Device m_device;
        vk::RenderPass m_renderPass;
   public:
        RenderPass();
        ~RenderPass();

        Result create(vk::Device device,vk::Format colorFormat,vk::Format depthFormat);
        void destroy();

        vk::RenderPass& getVkRenderPass();
    };
}
