#include<cmath>
#include<array>
#include<vector>
#include<iostream>
#include<unistd.h>
#include<string>
#include<vulkan/vk_cpp.h>
#include<vulkan/vulkan.h>
#include<GLFW/glfw3.h>
#include"simple_vulkan.h"

class TestApplication : public simpleVulkan::Application
{
    const vk::Format m_colorFormat = vk::Format::eB8G8R8A8Srgb;
    const vk::Format m_depthFormat = vk::Format::eD24UnormS8Uint;

    const float m_vertexes[3][2] = {{0.0,1.0f},{0.86,-0.5f},{-0.86,-0.5f}};
    float m_matrix[2][4] = {
        {1.0f,0.0f,0.0f,0.0f},
        {0.0f,1.0f,0.0f,0.0f}
    };

    const std::string m_vertexShaderName = "./vert.spv";
    const std::string m_fragShaderName = "./frag.spv";

    simpleVulkan::Instance* m_instance;
    simpleVulkan::Device* m_device;
    simpleVulkan::Queue* m_queue;
    simpleVulkan::CommandBuffers* m_cmdBuf;
    simpleVulkan::Swapchain* m_swapchain;
    simpleVulkan::Image* m_depthImage;
    simpleVulkan::RenderPass* m_renderPass;
    std::vector<simpleVulkan::Framebuffer*> m_FrameBuffers;
    simpleVulkan::Buffer* m_vertexBuffer;
    simpleVulkan::Buffer* m_matrixBuffer;
    simpleVulkan::DescriptorSets* m_descriptorSets;
    simpleVulkan::Shader* m_vertexShader;
    simpleVulkan::Shader* m_fragmentShader;
    simpleVulkan::GraphicsPipeline* m_pipeline;

    vk::SurfaceKHR m_windowSurface;
    vk::Semaphore m_semaphore;
    vk::Viewport m_viewport;
    vk::Rect2D m_scissor;


    uint32_t m_bufferIndex;
    uint32_t m_count;

    virtual bool initialize(
            const std::vector<const char*>& glfwExtensions,
            GLFWwindow* window) override
    {
        vk::Result result;

        std::vector<const char*> layers;
        m_instance = new simpleVulkan::Instance();
        m_instance->create("testApp",1,"testEngine",1,glfwExtensions,layers);

        m_device = new simpleVulkan::Device();
        m_device->create(m_instance->getVkInstance());

        m_queue = new simpleVulkan::Queue();
        m_queue->init(m_device->getVkDevice()); 

        m_cmdBuf = new simpleVulkan::CommandBuffers();
        m_cmdBuf->create(m_device->getVkDevice(),1);
        {
            //init CommandBufferInheritanceInfo
            vk::CommandBufferInheritanceInfo cmdBufInheritanceInfo;

            //init CommandBufferBeginInfo
            vk::CommandBufferBeginInfo cmdBufBeginInfo;
            cmdBufBeginInfo.pInheritanceInfo(&cmdBufInheritanceInfo);

            //begin CommandBuffer
            result = m_cmdBuf->getVkCommandBuffer(0).begin(&cmdBufBeginInfo);
        }

        //get WindowSurface
        glfwCreateWindowSurface(m_instance->getVkInstance(),window,nullptr,reinterpret_cast<VkSurfaceKHR*>(&m_windowSurface));

        m_swapchain = new simpleVulkan::Swapchain();
        m_swapchain->create(m_device->getVkDevice(),m_windowSurface,vk::ImageUsageFlagBits::eColorAttachment,m_colorFormat,getWidth(),getHeight());

        for(int i=0;i<m_swapchain->count();++i)
        {
            //init ImageMemoryBarrier
            vk::ImageMemoryBarrier barrier;
            barrier.srcAccessMask(vk::AccessFlagBits());
            barrier.dstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
            barrier.oldLayout(vk::ImageLayout::eUndefined);
            barrier.newLayout(vk::ImageLayout::ePresentSrcKHR);
            barrier.image(m_swapchain->getVkImage(i));
            barrier.subresourceRange().aspectMask(vk::ImageAspectFlagBits::eColor);
            barrier.subresourceRange().baseMipLevel(0);
            barrier.subresourceRange().levelCount(1);
            barrier.subresourceRange().baseArrayLayer(0);
            barrier.subresourceRange().layerCount(1);

            //layout SwapchainImage
            m_cmdBuf->getVkCommandBuffer(0).pipelineBarrier(
                    vk::PipelineStageFlagBits::eTopOfPipe,
                    vk::PipelineStageFlagBits::eTopOfPipe,
                    vk::DependencyFlagBits(),
                    0,
                    nullptr,
                    0,
                    nullptr,
                    1,
                    &barrier);
        }

        m_depthImage = new simpleVulkan::Image();
        m_depthImage->create(
                m_device->getVkDevice(),
                m_depthFormat,
                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                getWidth(),
                getHeight());


        m_renderPass = new simpleVulkan::RenderPass();
        m_renderPass->create(m_device->getVkDevice(),m_colorFormat,m_depthFormat);

        {
            m_FrameBuffers.resize(m_swapchain->count());
            for(size_t i=0;i<m_FrameBuffers.size();++i)
            {
                m_FrameBuffers[i] = new simpleVulkan::Framebuffer();
                m_FrameBuffers[i]->create(
                        m_device->getVkDevice(),
                        getWidth(),
                        getHeight(),
                        m_swapchain->getVkImageView(i),
                        m_depthImage->getVkImageView(),
                        m_renderPass->getVkRenderPass());
            }
        }

        //end CommandBuffer
        result = m_cmdBuf->getVkCommandBuffer(0).end();

        //init SubmitInfo
        m_queue->submit(m_cmdBuf->getVkCommandBuffers());

        //wait queue
        m_queue->wait();

        //read VertexShader
        std::vector<uint8_t> code(0);
        if(!readBinaryFile(m_vertexShaderName,code))
        {
            std::cout<<"could not read vertex shader!!" << std::endl;
        }

        m_vertexShader = new simpleVulkan::Shader();
        m_vertexShader->create(m_device->getVkDevice(),code.size(),code.data());

        //read FragShader
        code.clear();
        if(!readBinaryFile(m_fragShaderName,code))
        {
            std::cout<<"could not read fragment shader!!" << std::endl;
        }

        m_fragmentShader = new simpleVulkan::Shader();
        m_fragmentShader->create(m_device->getVkDevice(),code.size(),code.data());

        std::vector<vk::VertexInputBindingDescription> vertexBindings(1);
        std::vector<vk::VertexInputAttributeDescription> vertexAttributes(1);

        //init VertexBinding
        vertexBindings[0].binding(0);
        vertexBindings[0].inputRate(vk::VertexInputRate::eVertex);
        vertexBindings[0].stride(sizeof(float)*2);

        //init VertexAttributes
        vertexAttributes[0].binding(0);
        vertexAttributes[0].location(0);
        vertexAttributes[0].format(vk::Format::eR32G32Sfloat);
        vertexAttributes[0].offset(0);

        //create VertexBuffer
        m_vertexBuffer = new simpleVulkan::Buffer();
        m_vertexBuffer->create(m_device->getVkDevice(),vk::BufferUsageFlagBits::eVertexBuffer,sizeof(m_vertexes));

        //write VertexBuffer
        m_vertexBuffer->write(reinterpret_cast<const void*>(m_vertexes));

        //create FragmentBuffer
        m_matrixBuffer = new simpleVulkan::Buffer();
        m_matrixBuffer->create(m_device->getVkDevice(),vk::BufferUsageFlagBits::eUniformBuffer,sizeof(m_matrix));

        //write FragmentBuffer
        m_matrixBuffer->write(reinterpret_cast<const void*>(m_matrix));

        //init DescriptorSetLayoutBinding
        std::vector<vk::DescriptorSetLayoutBinding> layoutBinding(1);
        layoutBinding[0].binding(0);
        layoutBinding[0].descriptorType(vk::DescriptorType::eUniformBuffer);
        layoutBinding[0].stageFlags(vk::ShaderStageFlagBits::eVertex);
        layoutBinding[0].pImmutableSamplers(nullptr);
        layoutBinding[0].descriptorCount(1);

        m_descriptorSets = new simpleVulkan::DescriptorSets();
        m_descriptorSets->create(m_device->getVkDevice(),layoutBinding,1);

        //write DescriptorSet
        {

            //init DescriptorBufferInfo
            vk::DescriptorBufferInfo bufferInfo;
            bufferInfo.buffer(m_matrixBuffer->getVkBuffer());
            bufferInfo.offset(0);
            bufferInfo.range(sizeof(m_matrix));

            //init WriteDescriptorSet
            vk::WriteDescriptorSet writeSet;
            writeSet.dstSet(m_descriptorSets->getVkDescriptorSet(0));
            writeSet.dstBinding(0);
            writeSet.dstArrayElement(0);
            writeSet.descriptorCount(1);
            writeSet.descriptorType(vk::DescriptorType::eUniformBuffer);
            writeSet.pBufferInfo(&bufferInfo);

            //update DescriptorSet
            m_device->getVkDevice().updateDescriptorSets(1,&writeSet,0,nullptr);
        }

        //init Viewport
        m_viewport.x(0.0f);
        m_viewport.y(0.0f);
        m_viewport.width(getWidth());
        m_viewport.height(getHeight());
        m_viewport.minDepth(0.0f);
        m_viewport.maxDepth(1.0f);

        //init Scissor
        m_scissor.offset().x(0);
        m_scissor.offset().y(0);
        m_scissor.extent().width(getWidth());
        m_scissor.extent().height(getHeight());

        m_pipeline = new simpleVulkan::GraphicsPipeline();
        m_pipeline->create(
                m_device->getVkDevice(),
                m_vertexShader->getVkShaderModule(),
                m_fragmentShader->getVkShaderModule(),
                m_descriptorSets->getVkDescriptorSetLayout(),
                vertexBindings,
                vertexAttributes,
                m_viewport,
                m_scissor,
                m_renderPass->getVkRenderPass());
        {
            //init SemaphoreCreateInfo
            vk::SemaphoreCreateInfo semaphoreInfo;
            semaphoreInfo.flags(vk::SemaphoreCreateFlagBits());

            //create Semaphore
            result = m_device->getVkDevice().createSemaphore(&semaphoreInfo,nullptr,&m_semaphore);
        }

        //acquire NextImage
        result = m_device->getVkDevice().acquireNextImageKHR(m_swapchain->getVkSwapchainKHR(),4000000,m_semaphore,nullptr,&m_bufferIndex);
        return true;
    }
    virtual void finalize() override
    {
        std::cout << "end" << std::endl;
    }

    virtual bool render() override
    {
        vk::Result result;
        float rate = 1000;
        m_matrix[0][0] =  std::cos(m_count / rate);
        m_matrix[0][1] =  std::sin(m_count / rate);
        m_matrix[1][0] = -std::sin(m_count / rate);
        m_matrix[1][1] =  std::cos(m_count / rate);
        m_matrixBuffer->write(reinterpret_cast<void*>(&m_matrix));
        {
            //init CommandBufferInheritanceInfo
            vk::CommandBufferInheritanceInfo inheritanceInfo;
            inheritanceInfo.renderPass(m_renderPass->getVkRenderPass()); //debug
            inheritanceInfo.subpass(0); //debug
            inheritanceInfo.framebuffer(m_FrameBuffers[m_bufferIndex]->getVkFrameBuffer()); //debug
            inheritanceInfo.occlusionQueryEnable(false);
            inheritanceInfo.queryFlags(vk::QueryControlFlagBits());
            inheritanceInfo.pipelineStatistics(vk::QueryPipelineStatisticFlagBits());

            //init CommandBufferBeginInfo
            vk::CommandBufferBeginInfo beginInfo;
            beginInfo.pInheritanceInfo(&inheritanceInfo);

            //begin CommandBuffer
            m_cmdBuf->getVkCommandBuffer(0).begin(&beginInfo);
        }


        {
            //clear color
            vk::ClearColorValue clearColor(std::array<float,4>{0.0f,0.0f,1.0f,1.0f});
            vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor,0,1,0,1);
            m_cmdBuf->getVkCommandBuffer(0).clearColorImage(m_swapchain->getVkImage(m_bufferIndex),vk::ImageLayout::eColorAttachmentOptimal,&clearColor,1,&range);
        }

        {
            //clear depth
            vk::ClearDepthStencilValue clearDepth(1.0f,0);
            vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eDepth,0,1,0,1);
            m_cmdBuf->getVkCommandBuffer(0).clearDepthStencilImage(m_depthImage->getVkImage(),vk::ImageLayout::eDepthStencilAttachmentOptimal,&clearDepth,1,&range);
        }

        {
            //pipelineBarrier
            vk::ImageMemoryBarrier barrier;
            barrier.srcAccessMask(vk::AccessFlagBits::eMemoryRead);
            barrier.dstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
            barrier.oldLayout(vk::ImageLayout::ePresentSrcKHR);
            barrier.newLayout(vk::ImageLayout::eColorAttachmentOptimal);
            barrier.srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
            barrier.dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
            barrier.subresourceRange().aspectMask(vk::ImageAspectFlagBits::eColor);
            barrier.subresourceRange().baseMipLevel(0);
            barrier.subresourceRange().layerCount(1);
            barrier.image(m_swapchain->getVkImage(m_bufferIndex));
            m_cmdBuf->getVkCommandBuffer(0).pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,vk::PipelineStageFlagBits::eColorAttachmentOutput,vk::DependencyFlagBits(),0,nullptr,0,nullptr,1,&barrier);
        }

        {
            //clear image
            vk::ClearColorValue clearColor(std::array<float,4>{0.0f,0.0f,0.0f,1.0f});
            vk::ClearDepthStencilValue clearDepth(1.0f,0.0f);
            vk::ClearValue clearValues[2];
            clearValues[0].color(clearColor);
            clearValues[1].depthStencil(clearDepth);

            //init RenderPassBeginInfo
            vk::RenderPassBeginInfo passInfo;
            passInfo.renderPass(m_renderPass->getVkRenderPass());
            passInfo.framebuffer(m_FrameBuffers[m_bufferIndex]->getVkFrameBuffer());
            passInfo.renderArea().offset().x(0);
            passInfo.renderArea().offset().y(0);
            passInfo.renderArea().extent().width(getWidth());
            passInfo.renderArea().extent().height(getHeight());
            passInfo.clearValueCount(2);
            passInfo.pClearValues(clearValues);

            //begin RenderPass
            m_cmdBuf->getVkCommandBuffer(0).beginRenderPass(&passInfo,vk::SubpassContents::eInline);
        }

        //bind Pipeline
        m_cmdBuf->getVkCommandBuffer(0).bindPipeline(vk::PipelineBindPoint::eGraphics,m_pipeline->getVkPipeline());

        //bind DescriptorSet
        m_cmdBuf->getVkCommandBuffer(0).bindDescriptorSets(vk::PipelineBindPoint::eGraphics,m_pipeline->getVkPipelineLayout(),0,1,&m_descriptorSets->getVkDescriptorSet(0),0,nullptr);

        //set Viewport
        m_cmdBuf->getVkCommandBuffer(0).setViewport(0,1,&m_viewport);

        //set Scissor
        m_cmdBuf->getVkCommandBuffer(0).setScissor(0,1,&m_scissor);

        //bind VertexBuffer
        vk::DeviceSize offset(0);
        m_cmdBuf->getVkCommandBuffer(0).bindVertexBuffers(0,1,&m_vertexBuffer->getVkBuffer(),&offset);

        //draw
        m_cmdBuf->getVkCommandBuffer(0).draw(3,1,0,0);

        //end RenderPass
        m_cmdBuf->getVkCommandBuffer(0).endRenderPass();

        {
            //pipelineBarrier
            vk::ImageMemoryBarrier barrier;
            barrier.srcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
            barrier.dstAccessMask(vk::AccessFlagBits::eMemoryRead);
            barrier.oldLayout(vk::ImageLayout::eColorAttachmentOptimal);
            barrier.newLayout(vk::ImageLayout::ePresentSrcKHR);
            barrier.srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
            barrier.dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
            barrier.subresourceRange().aspectMask(vk::ImageAspectFlagBits::eColor);
            barrier.subresourceRange().baseMipLevel(0);
            barrier.subresourceRange().layerCount(1);
            barrier.image(m_swapchain->getVkImage(m_bufferIndex));
            m_cmdBuf->getVkCommandBuffer(0).pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,vk::PipelineStageFlagBits::eBottomOfPipe,vk::DependencyFlagBits(),0,nullptr,0,nullptr,1,&barrier);

        }

        //end CommandBuffer
        m_cmdBuf->getVkCommandBuffer(0).end();

        m_queue->submit(m_cmdBuf->getVkCommandBuffers());
        
        //wait Queue
        m_queue->wait();

        m_queue->present(m_swapchain->getVkSwapchainKHR(),m_bufferIndex);

        //acquire next Image
        result = m_device->getVkDevice().acquireNextImageKHR(m_swapchain->getVkSwapchainKHR(),4000000,m_semaphore,nullptr,&m_bufferIndex);
        std::cout << glfwGetTime() <<  std::endl;
        ++m_count;
        return true;
    }
};


int main()
{
    simpleVulkan::Application* app = new TestApplication();
    if(app->create("test",400,400))
    {
        app->run();
        app->destroy();
    }
}
