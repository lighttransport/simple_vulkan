#include<cmath>
#include<array>
#include<vector>
#include<thread>
#include<iostream>
#include<thread>
#include<string>
#include<vulkan/vk_cpp.h>
#include<vulkan/vulkan.h>
#include<GLFW/glfw3.h>
#include"simple_vulkan.h"


class MultiGPUApplication : public simpleVulkan::Application
{
	const std::string m_vertexShaderName = "./vert.spv";
	const std::string m_fragShaderName = "./frag.spv";

	simpleVulkan::Instance m_instance;
	vk::SurfaceKHR m_windowSurface;

	struct CalcGPU
	{
        simpleVulkan::Device device;
		simpleVulkan::Queue queue;
		simpleVulkan::CommandBuffers cmdBuf;
        simpleVulkan::Image colorImage;
		simpleVulkan::Image depthImage;
		simpleVulkan::RenderPass renderPass;
        simpleVulkan::Framebuffer framebuffer;
		simpleVulkan::Buffer vertexBuffer;
		simpleVulkan::Buffer matrixBuffer;
		simpleVulkan::DescriptorSets descriptorSets;
		simpleVulkan::Shader vertexShader;
		simpleVulkan::Shader fragmentShader;
		simpleVulkan::GraphicsPipeline pipeline;
		vk::Semaphore semaphore;
	} m_calcGPU[2];

    struct RenderGPU
    {
        simpleVulkan::Queue queue;
        simpleVulkan::CommandBuffers cmdBuf;
        simpleVulkan::Image colorImage;
        simpleVulkan::RenderPass renderpass;
        simpleVulkan::Framebuffer framebuffer;
        simpleVulkan::Buffer vertexBuffer;
        simpleVulkan::DescriptorSets descriptorSets;
        simpleVulkan::Shader vertexShader;
        simpleVulkan::Shader fragmentShader;
        simpleVulkan::GraphicsPipeline pipeline;
    } renderGPU;

	vk::Format m_colorFormat;
	vk::Format m_depthFormat;
	std::vector<vk::VertexInputBindingDescription> m_vertexBindings;
	std::vector<vk::VertexInputAttributeDescription> m_vertexAttributes;
	vk::Viewport m_viewport;
	vk::Viewport m_viewport2;
	vk::Rect2D m_scissor;
	vk::Rect2D m_scissor2;

	float m_vertexes[3][2];
	float m_matrix[2][4];
	uint32_t m_bufferIndex;
	uint32_t m_count;


public:
	using simpleVulkan::Application::Application;
private:

    void initRenderingData()
    {
        //init Format
        m_colorFormat = vk::Format::eR8G8B8A8Snorm;
        m_depthFormat = vk::Format::eD24UnormS8Uint;

		//init VertexBinding
		m_vertexBindings.resize(1);
		m_vertexBindings[0].binding(0);
		m_vertexBindings[0].inputRate(vk::VertexInputRate::eVertex);
		m_vertexBindings[0].stride(sizeof(float) * 2);

		//init VertexAttributes
		m_vertexAttributes.resize(1);
		m_vertexAttributes[0].binding(0);
		m_vertexAttributes[0].location(0);
		m_vertexAttributes[0].format(vk::Format::eR32G32Sfloat);
		m_vertexAttributes[0].offset(0);

		//init Viewport
		m_viewport.x(0.0f);
		m_viewport.y(0.0f);
		m_viewport.width((float)getWidth());
		m_viewport.height((float)getHeight());
		m_viewport.minDepth(0.0f);
		m_viewport.maxDepth(1.0f);

		//init Scissor
		m_scissor.offset().x(0);
		m_scissor.offset().y(0);
		m_scissor.extent().width(getWidth());
		m_scissor.extent().height(getHeight());

        //init Vertexes position
		m_vertexes[0][0] =  0.0f;
		m_vertexes[0][1] =  1.0f;
		m_vertexes[1][0] =  0.86f;
		m_vertexes[1][1] = -0.5f;
		m_vertexes[2][0] = -0.86f;
		m_vertexes[2][1] = -0.5f;

    }

    void initDevice(CalcGPU& gpu,size_t deviceIndex)
    {
		gpu.device.create(m_instance.getVkInstance(),deviceIndex, getValidateFlag());
		gpu.queue.init(gpu.device.getVkDevice());
		gpu.cmdBuf.create(gpu.device.getVkDevice(),2);
    }

	void initFramebuffer(CalcGPU& gpu )
	{
		gpu.cmdBuf.begin(0);

        gpu.colorImage.create(
                gpu.device.getVkDevice(),
                m_colorFormat,
                vk::ImageUsageFlagBits::eColorAttachment,
                getWidth(),
                getHeight());

		gpu.depthImage.create(
            gpu.device.getVkDevice(),
			m_depthFormat,
			vk::ImageUsageFlagBits::eDepthStencilAttachment,
			getWidth(),
			getHeight());

		gpu.cmdBuf.end(0);

		//init SubmitInfo
		gpu.queue.submit(gpu.cmdBuf.getVkCommandBuffer(0));

		//wait queue
		gpu.queue.wait();

		gpu.renderPass.create(gpu.device.getVkDevice(), m_colorFormat, m_depthFormat);

        gpu.framebuffer.create(
                gpu.device.getVkDevice(),
                getWidth(),
                getHeight(),
                gpu.colorImage.getVkImageView(),
                gpu.depthImage.getVkImageView(),
                gpu.renderPass.getVkRenderPass());
	}

	void initShader(CalcGPU& gpu)
	{
		//read VertexShader
		std::vector<uint8_t> code(0);
		if (!readBinaryFile(m_vertexShaderName, code))
		{
			std::cout << "could not read vertex shader!!" << std::endl;
		}

		gpu.vertexShader.create(gpu.device.getVkDevice(), code.size(), code.data());

		//read FragShader
		code.clear();
		if (!readBinaryFile(m_fragShaderName, code))
		{
			std::cout << "could not read fragment shader!!" << std::endl;
		}

		gpu.fragmentShader.create(gpu.device.getVkDevice(), code.size(), code.data());
	}

	void initBuffer(CalcGPU& gpu)
	{

		//create VertexBuffer
		gpu.vertexBuffer.create(gpu.device.getVkPhysicalDevice(), gpu.device.getVkDevice(), vk::BufferUsageFlagBits::eVertexBuffer, sizeof(m_vertexes));

		//write VertexBuffer
		gpu.vertexBuffer.write(reinterpret_cast<const void*>(m_vertexes));

		//create MatrixBuffer
		gpu.matrixBuffer.create(gpu.device.getVkPhysicalDevice(), gpu.device.getVkDevice(), vk::BufferUsageFlagBits::eUniformBuffer, sizeof(m_matrix));

		//write MatrixBuffer
		gpu.matrixBuffer.write(reinterpret_cast<const void*>(m_matrix));

	}

	void initDescriptor(CalcGPU& gpu)
	{
		//init DescriptorSetLayoutBinding
		std::vector<vk::DescriptorSetLayoutBinding> layoutBinding(1);
		layoutBinding[0].binding(0);
		layoutBinding[0].descriptorType(vk::DescriptorType::eUniformBuffer);
		layoutBinding[0].stageFlags(vk::ShaderStageFlagBits::eVertex);
		layoutBinding[0].pImmutableSamplers(nullptr);
		layoutBinding[0].descriptorCount(1);

		gpu.descriptorSets.create(gpu.device.getVkDevice(), layoutBinding, 1);

		//write DescriptorSet
		{

			//init DescriptorBufferInfo
			vk::DescriptorBufferInfo bufferInfo;
			bufferInfo.buffer(gpu.matrixBuffer.getVkBuffer());
			bufferInfo.offset(0);
			bufferInfo.range(sizeof(m_matrix));

			//init WriteDescriptorSet
			vk::WriteDescriptorSet writeSet;
			writeSet.dstSet(gpu.descriptorSets.getVkDescriptorSet(0));
			writeSet.dstBinding(0);
			writeSet.dstArrayElement(0);
			writeSet.descriptorCount(1);
			writeSet.descriptorType(vk::DescriptorType::eUniformBuffer);
			writeSet.pBufferInfo(&bufferInfo);

			//update DescriptorSet
			gpu.device.getVkDevice().updateDescriptorSets(1, &writeSet, 0, nullptr);
		}

	}

	void initPipeline(CalcGPU& gpu)
    {
		//create Pipeline
		gpu.pipeline.create(
			gpu.device.getVkDevice(),
			gpu.vertexShader.getVkShaderModule(),
			gpu.fragmentShader.getVkShaderModule(),
			gpu.descriptorSets.getVkDescriptorSetLayout(),
			m_vertexBindings,
			m_vertexAttributes,
			m_viewport,
			m_scissor,
			gpu.renderPass.getVkRenderPass());
	}

    void initCalcGPU(CalcGPU& gpu,size_t deviceIndex)
    {
        initDevice(gpu,deviceIndex);

		initFramebuffer(gpu);

		initShader(gpu);

		initBuffer(gpu);

		initDescriptor(gpu);

		initPipeline(gpu);

        //init SemaphoreCreateInfo
        vk::SemaphoreCreateInfo semaphoreInfo;

        //create Semaphore
        gpu.device.getVkDevice().createSemaphore(
                &semaphoreInfo,
                nullptr,
                &gpu.semaphore);
    }

    void updateRenderingData()
    {
		float rate = 100 / (3.14f * 2.0f);
		m_matrix[0][0] = std::cos(m_count / rate);
		m_matrix[0][1] = std::sin(m_count / rate);
		m_matrix[1][0] = -std::sin(m_count / rate);
		m_matrix[1][1] = std::cos(m_count / rate);
    }

    void renderCalcGPU(CalcGPU& gpu)
    {
		gpu.matrixBuffer.write(reinterpret_cast<void*>(&m_matrix));

		{
			//init CommandBufferInheritanceInfo
			vk::CommandBufferInheritanceInfo inheritanceInfo;
			inheritanceInfo.renderPass(gpu.renderPass.getVkRenderPass());
			inheritanceInfo.subpass(0);
			inheritanceInfo.framebuffer(gpu.framebuffer.getVkFrameBuffer());
			inheritanceInfo.occlusionQueryEnable(false);
			inheritanceInfo.queryFlags(vk::QueryControlFlagBits());
			inheritanceInfo.pipelineStatistics(vk::QueryPipelineStatisticFlagBits());

			//init CommandBufferBeginInfo
			vk::CommandBufferBeginInfo beginInfo;
			beginInfo.pInheritanceInfo(&inheritanceInfo);

			//begin CommandBuffer
			gpu.cmdBuf.getVkCommandBuffer(1).begin(&beginInfo);

            //pipelineBarrier
            gpu.colorImage.barrier(
                    gpu.cmdBuf.getVkCommandBuffer(1),
                    vk::AccessFlagBits::eMemoryRead,
                    vk::AccessFlagBits::eColorAttachmentWrite,
                    vk::ImageLayout::eGeneral,
                    vk::ImageLayout::eColorAttachmentOptimal,
                    vk::PipelineStageFlagBits::eAllCommands,
                    vk::PipelineStageFlagBits::eTopOfPipe
                    );

		}

		{
			//clear color
			vk::ClearColorValue clearColor(std::array<float, 4>{0.0f, 1.0f, 0.0f, 1.0f});
			vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
			gpu.cmdBuf.getVkCommandBuffer(1).clearColorImage(gpu.colorImage.getVkImage(), vk::ImageLayout::eGeneral, &clearColor, 1, &range);
		}

		{
			//clear depth
			vk::ClearDepthStencilValue clearDepth(1.0f, 0);
			vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1);
			gpu.cmdBuf.getVkCommandBuffer(1).clearDepthStencilImage(gpu.depthImage.getVkImage(), vk::ImageLayout::eGeneral, &clearDepth, 1, &range);
		}

		{
			//clear image
			vk::ClearColorValue clearColor(std::array<float, 4>{1.0f, 0.0f, 0.0f, 1.0f});
			vk::ClearDepthStencilValue clearDepth(1.0f, 0.0f);
			vk::ClearValue clearValues[2];
			clearValues[0].color(clearColor);
			clearValues[1].depthStencil(clearDepth);

			//init RenderPassBeginInfo
			vk::RenderPassBeginInfo passInfo;
			passInfo.renderPass(gpu.renderPass.getVkRenderPass());
			passInfo.framebuffer(gpu.framebuffer.getVkFrameBuffer());
			passInfo.renderArea().offset().x(0);
			passInfo.renderArea().offset().y(0);
			passInfo.renderArea().extent().width(getWidth());
			passInfo.renderArea().extent().height(getHeight());
			passInfo.clearValueCount(2);
			passInfo.pClearValues(clearValues);

			//begin RenderPass
			gpu.cmdBuf.getVkCommandBuffer(1).beginRenderPass(&passInfo, vk::SubpassContents::eInline);
		}

		//bind Pipeline
		gpu.cmdBuf.getVkCommandBuffer(1).bindPipeline(vk::PipelineBindPoint::eGraphics, gpu.pipeline.getVkPipeline());

		//bind DescriptorSet
		gpu.cmdBuf.getVkCommandBuffer(1).bindDescriptorSets(vk::PipelineBindPoint::eGraphics, gpu.pipeline.getVkPipelineLayout(), 0, 1, &gpu.descriptorSets.getVkDescriptorSet(0), 0, nullptr);

		//bind VertexBuffer
		vk::DeviceSize offset(0);
		gpu.cmdBuf.getVkCommandBuffer(1).bindVertexBuffers(0, 1, &gpu.vertexBuffer.getVkBuffer(), &offset);

		//draw
		gpu.cmdBuf.getVkCommandBuffer(1).draw(3, 1, 0, 0);

		//end RenderPass
		gpu.cmdBuf.getVkCommandBuffer(1).endRenderPass();

        //pipelineBarrier
        gpu.colorImage.barrier(
                gpu.cmdBuf.getVkCommandBuffer(1),
                vk::AccessFlagBits::eColorAttachmentWrite,
                vk::AccessFlagBits::eMemoryRead,
                vk::ImageLayout::eColorAttachmentOptimal,
                vk::ImageLayout::eGeneral,
                vk::PipelineStageFlagBits::eColorAttachmentOutput,
                vk::PipelineStageFlagBits::eBottomOfPipe
                );

		//end CommandBuffer
		gpu.cmdBuf.getVkCommandBuffer(1).end();

		//submit Queue
		gpu.queue.submit(gpu.cmdBuf.getVkCommandBuffer(1), { gpu.semaphore });

    }

    void waitCalcGPU(CalcGPU& gpu)
    {
		//wait Queue
		gpu.queue.wait();
        gpu.device.getVkDevice().waitIdle();
    }

	virtual bool initialize()
	{
		setInterval(std::chrono::milliseconds(100));
		vk::Result result;

        initRenderingData();

		m_instance = getInstance();

        initCalcGPU(m_calcGPU[0],0);
        //initCalcGPU(m_calcGPU[1],1);

		return true;

	}
	virtual void finalize() override
	{
	}

	virtual bool render() override
	{
        //update data
        updateRenderingData();
        
        //render
        renderCalcGPU(m_calcGPU[0]);
        renderCalcGPU(m_calcGPU[1]);

        //wain Device
        waitCalcGPU(m_calcGPU[0]);
        waitCalcGPU(m_calcGPU[1]);


		std::cout << "BufferIndex:" << m_bufferIndex << std::endl;
        std::cout << "Time:" << glfwGetTime() << std::endl;
		++m_count;
		return true;
	}
};


int main(int argc, char **argv)
{
	bool validate = true; // Use vulkan validation layer + debug report?
	if (argc > 1)
	{
		if (strcmp("-novalidate", argv[1]) == 0) {
			validate = false;
		}
	}
	simpleVulkan::Application* app = new MultiGPUApplication();
	if (app->create("test", 800, 400, validate))
	{
		app->run();
		app->destroy();
	}
}
