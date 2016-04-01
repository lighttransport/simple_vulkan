#pragma once
#include<vulkan/vk_cpp.h>
#include"device.h"
#include"shader.h"
#include"render_pass.h"

namespace simpleVulkan
{
    class GraphicsPipeline
    {
        vk::Device m_device;
        vk::PipelineCache m_pipelineCache;
        vk::PipelineLayout m_pipelineLayout;
        vk::Pipeline m_pipeline;
   public:
        GraphicsPipeline();
        ~GraphicsPipeline();
        Result create(
                vk::Device device,
                vk::ShaderModule vertexShader,
                vk::ShaderModule fragmentShader,
                vk::DescriptorSetLayout setLayout,
                const std::vector<vk::VertexInputBindingDescription>& vertexBindings,
                const std::vector<vk::VertexInputAttributeDescription>& vertexAttributes,
                vk::Viewport viewport,
                vk::Rect2D scissor,
                vk::RenderPass renderPass);
        void destroy();

        vk::PipelineLayout& getVkPipelineLayout();
        vk::Pipeline& getVkPipeline();
    };
}
