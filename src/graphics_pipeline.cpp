#include"graphics_pipeline.h"

namespace simpleVulkan
{
    GraphicsPipeline::GraphicsPipeline()
    {
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
    }

    Result GraphicsPipeline::create(
            vk::Device device,
            vk::ShaderModule vertexShader,
            vk::ShaderModule fragmentShader,
            vk::DescriptorSetLayout setLayout,
            const std::vector<vk::VertexInputBindingDescription>& vertexBindings,
            const std::vector<vk::VertexInputAttributeDescription>& vertexAttributes,
            vk::Viewport viewport,
            vk::Rect2D scissor,
            vk::RenderPass renderPass)
    {
        vk::Result result;

        m_device = device;


        //init PipelineCacheCreateInfo
        vk::PipelineCacheCreateInfo cacheInfo;
        cacheInfo.flags(vk::PipelineCacheCreateFlagBits());
        cacheInfo.initialDataSize(0);
        cacheInfo.pInitialData(nullptr);

        //create PipelineCache
        result = m_device.createPipelineCache(
                &cacheInfo,
                nullptr,
                &m_pipelineCache);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

        //init PipelineShaderStageCreateInfo
        vk::PipelineShaderStageCreateInfo stageInfos[2];
        stageInfos[0].flags(vk::PipelineShaderStageCreateFlagBits());
        stageInfos[0].stage(vk::ShaderStageFlagBits::eVertex);
        stageInfos[0].module(vertexShader);
        stageInfos[0].pName("main");
        stageInfos[0].pSpecializationInfo(nullptr);
        stageInfos[1].flags(vk::PipelineShaderStageCreateFlagBits());
        stageInfos[1].stage(vk::ShaderStageFlagBits::eFragment);
        stageInfos[1].module(fragmentShader);
        stageInfos[1].pName("main");
        stageInfos[1].pSpecializationInfo(nullptr);

        //init PipelineLayoutCreateInfo
        vk::PipelineLayoutCreateInfo layoutInfo;
        layoutInfo.pushConstantRangeCount(0);
        layoutInfo.pPushConstantRanges(nullptr);
        layoutInfo.setLayoutCount(1);
        layoutInfo.pSetLayouts(&setLayout);

        result = m_device.createPipelineLayout(
                &layoutInfo,
                nullptr,
                &m_pipelineLayout);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

        //init VertexInputStateCreateInfo
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
        vertexInputInfo.flags(vk::PipelineVertexInputStateCreateFlagBits());
        vertexInputInfo.vertexBindingDescriptionCount(vertexBindings.size());
        vertexInputInfo.pVertexBindingDescriptions(vertexBindings.data());
        vertexInputInfo.vertexAttributeDescriptionCount(vertexAttributes.size());
        vertexInputInfo.pVertexAttributeDescriptions(vertexAttributes.data());

        //init PipelineInputAssemblyStateCreateInfo
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        inputAssemblyInfo.flags(vk::PipelineInputAssemblyStateCreateFlagBits());
        inputAssemblyInfo.topology(vk::PrimitiveTopology::eTriangleList);
        inputAssemblyInfo.primitiveRestartEnable(false);

        //init PipelineViewportStateCreateInfo
        vk::PipelineViewportStateCreateInfo viewportInfo;
        viewportInfo.flags(vk::PipelineViewportStateCreateFlagBits());
        viewportInfo.viewportCount(1);
        viewportInfo.pViewports(&viewport);
        viewportInfo.scissorCount(1);
        viewportInfo.pScissors(&scissor);

        //init PipelineRasterizationStateCreateInfo
        vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
        rasterizationInfo.flags(vk::PipelineRasterizationStateCreateFlagBits());
        rasterizationInfo.depthClampEnable(false);
        rasterizationInfo.rasterizerDiscardEnable(false);
        rasterizationInfo.polygonMode(vk::PolygonMode::eFill);
        rasterizationInfo.cullMode(vk::CullModeFlagBits::eNone);
        rasterizationInfo.frontFace(vk::FrontFace::eCounterClockwise);
        rasterizationInfo.depthBiasEnable(false);
        rasterizationInfo.depthBiasConstantFactor(0.0f);
        rasterizationInfo.depthBiasClamp(0.0f);
        rasterizationInfo.depthBiasSlopeFactor(0.0f);
        rasterizationInfo.lineWidth(1.0f);

        //init PipelineMultisampleStateCreateInfo
        vk::PipelineMultisampleStateCreateInfo multisampleInfo;
        multisampleInfo.flags(vk::PipelineMultisampleStateCreateFlagBits());
        multisampleInfo.rasterizationSamples(vk::SampleCountFlagBits::e1);
        multisampleInfo.sampleShadingEnable(false);
        multisampleInfo.minSampleShading(0.0f);
        multisampleInfo.pSampleMask(nullptr);
        multisampleInfo.alphaToCoverageEnable(false);
        multisampleInfo.alphaToOneEnable(false);

        //init StencilOpState
        vk::StencilOpState stencilState;
        stencilState.failOp(vk::StencilOp::eKeep);
        stencilState.passOp(vk::StencilOp::eKeep);
        stencilState.depthFailOp(vk::StencilOp::eKeep);
        stencilState.compareOp(vk::CompareOp::eNever);
        stencilState.compareMask(0);
        stencilState.writeMask(0);
        stencilState.reference(0);

        //init PipelineDepthStencilStateCreateInfo
        vk::PipelineDepthStencilStateCreateInfo depthInfo;
        depthInfo.flags(vk::PipelineDepthStencilStateCreateFlagBits());
        depthInfo.depthTestEnable(true); //debug
        depthInfo.depthWriteEnable(true);
        depthInfo.depthCompareOp(vk::CompareOp::eLessOrEqual);
        depthInfo.depthBoundsTestEnable(false);
        depthInfo.stencilTestEnable(false);
        depthInfo.front(stencilState);
        depthInfo.back(stencilState);
        depthInfo.minDepthBounds(0.0f);
        depthInfo.maxDepthBounds(0.0f);

        //init PipelineColorBlendAttachmentState
        vk::PipelineColorBlendAttachmentState blendState;
        blendState.blendEnable(false);
        blendState.srcColorBlendFactor(vk::BlendFactor::eZero);
        blendState.dstColorBlendFactor(vk::BlendFactor::eZero);
        blendState.colorBlendOp(vk::BlendOp::eAdd);
        blendState.srcAlphaBlendFactor(vk::BlendFactor::eZero);
        blendState.dstAlphaBlendFactor(vk::BlendFactor::eZero);
        blendState.alphaBlendOp(vk::BlendOp::eAdd);
        blendState.colorWriteMask(
                vk::ColorComponentFlagBits::eR |
                vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB |
                vk::ColorComponentFlagBits::eA );

        //init PipelineColorBlendStateCreateInfo
        vk::PipelineColorBlendStateCreateInfo blendInfo;
        blendInfo.flags(vk::PipelineColorBlendStateCreateFlagBits());
        blendInfo.logicOpEnable(false);
        blendInfo.logicOp(vk::LogicOp::eClear);
        blendInfo.attachmentCount(1);
        blendInfo.pAttachments(&blendState);
        blendInfo.blendConstants(std::array<float,4>{0.0f,0.0f,0.0f,0.0f});

        //init GraphicPipelineCreateInfo
        vk::GraphicsPipelineCreateInfo pipelineInfo;
        pipelineInfo.flags(vk::PipelineCreateFlagBits());
        pipelineInfo.stageCount(2);
        pipelineInfo.pStages(stageInfos);
        pipelineInfo.pVertexInputState(&vertexInputInfo);
        pipelineInfo.pInputAssemblyState(&inputAssemblyInfo);
        pipelineInfo.pTessellationState(nullptr);
        pipelineInfo.pViewportState(&viewportInfo);
        pipelineInfo.pRasterizationState(&rasterizationInfo);
        pipelineInfo.pMultisampleState(&multisampleInfo);
        pipelineInfo.pDepthStencilState(&depthInfo);
        pipelineInfo.pColorBlendState(&blendInfo);
        pipelineInfo.pDynamicState(nullptr);
        pipelineInfo.layout(m_pipelineLayout);
        pipelineInfo.renderPass(renderPass);
        pipelineInfo.subpass(0);
        pipelineInfo.basePipelineHandle(0);
        pipelineInfo.basePipelineIndex(0);

        //create GraphicPipeline
        result = m_device.createGraphicsPipelines(
                m_pipelineCache,
                1,
                &pipelineInfo,
                nullptr,
                &m_pipeline);
        return result;
    }

void GraphicsPipeline::destroy()
{
    m_device.destroyPipeline(m_pipeline,nullptr);
    m_device.destroyPipelineLayout(m_pipelineLayout,nullptr);
    m_device.destroyPipelineCache(m_pipelineCache,nullptr);
}

vk::PipelineLayout& GraphicsPipeline::getVkPipelineLayout()
{
    return m_pipelineLayout;
}

    vk::Pipeline& GraphicsPipeline::getVkPipeline()
    {
        return m_pipeline;
    }
}
