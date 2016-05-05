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


class TriangleApplication : public simpleVulkan::Application
{
	vk::Format m_colorFormat;
	vk::Format m_depthFormat = vk::Format::eD24UnormS8Uint;

	float m_vertexes[3][2];
	float m_matrix[2][4];

	const std::string m_vertexShaderName = "./vert.spv";
	const std::string m_fragShaderName = "./frag.spv";

	simpleVulkan::Instance m_instance;
	simpleVulkan::Devices m_devices;
	simpleVulkan::Queue m_queue;
	simpleVulkan::CommandBuffers m_cmdBuf;
	simpleVulkan::Swapchain m_swapchain;
	simpleVulkan::Image m_depthImage;
	simpleVulkan::RenderPass m_renderPass;
	std::vector<simpleVulkan::Framebuffer> m_FrameBuffers;
	simpleVulkan::Buffer m_vertexBuffer;
	simpleVulkan::Buffer m_matrixBuffer;
	simpleVulkan::DescriptorSets m_descriptorSets;
	simpleVulkan::Shader m_vertexShader;
	simpleVulkan::Shader m_fragmentShader;
	simpleVulkan::GraphicsPipeline m_pipeline;
	simpleVulkan::GraphicsPipeline m_pipeline2;

	vk::SurfaceKHR m_windowSurface;
	vk::Semaphore m_semaphore;
	std::vector<vk::VertexInputBindingDescription> m_vertexBindings;
	std::vector<vk::VertexInputAttributeDescription> m_vertexAttributes;
	vk::Viewport m_viewport;
	vk::Viewport m_viewport2;
	vk::Rect2D m_scissor;
	vk::Rect2D m_scissor2;


	uint32_t m_bufferIndex;
	uint32_t m_count;

public:
	using simpleVulkan::Application::Application;
private:

	void initFramebuffer()
	{

		m_cmdBuf.begin(0);

		m_swapchain.create(m_devices.getVkPhysicalDevice(0), m_devices.getVkDevice(0), m_instance.getSurface(), vk::ImageUsageFlagBits::eColorAttachment, getWidth(), getHeight());
		m_colorFormat = m_swapchain.getFormat();

		for (int i = 0; i < m_swapchain.count(); ++i)
		{
			//init ImageMemoryBarrier
			vk::ImageMemoryBarrier barrier;
			barrier.srcAccessMask(vk::AccessFlagBits());
			barrier.dstAccessMask(vk::AccessFlagBits::eMemoryRead);
			barrier.oldLayout(vk::ImageLayout::eUndefined);
			barrier.newLayout(vk::ImageLayout::ePresentSrcKHR);
			barrier.image(m_swapchain.getVkImage(i));
			barrier.subresourceRange().aspectMask(vk::ImageAspectFlagBits::eColor);
			barrier.subresourceRange().baseMipLevel(0);
			barrier.subresourceRange().levelCount(1);
			barrier.subresourceRange().baseArrayLayer(0);
			barrier.subresourceRange().layerCount(1);

			//layout SwapchainImage
			m_cmdBuf.getVkCommandBuffer(0).pipelineBarrier(
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

		m_depthImage.create(
			m_devices.getVkDevice(0),
			m_depthFormat,
			vk::ImageUsageFlagBits::eDepthStencilAttachment,
			getWidth(),
			getHeight());

		//init ImageMemoryBarrier
		vk::ImageMemoryBarrier barrier;
		barrier.srcAccessMask(vk::AccessFlagBits());
		barrier.dstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead);
		barrier.oldLayout(vk::ImageLayout::eUndefined);
		barrier.newLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
		barrier.image(m_depthImage.getVkImage());
		barrier.subresourceRange().aspectMask(vk::ImageAspectFlagBits::eDepth);
		barrier.subresourceRange().baseMipLevel(0);
		barrier.subresourceRange().levelCount(1);
		barrier.subresourceRange().baseArrayLayer(0);
		barrier.subresourceRange().layerCount(1);

		//layout DepthImage
		m_cmdBuf.getVkCommandBuffer(0).pipelineBarrier(

			vk::PipelineStageFlagBits::eTopOfPipe,
			vk::PipelineStageFlagBits::eTopOfPipe,
			vk::DependencyFlagBits(),
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier);


		m_cmdBuf.end(0);

		//init SubmitInfo
		m_queue.submit(m_cmdBuf.getVkCommandBuffer(0));

		//wait queue
		m_queue.wait();

		m_renderPass.create(m_devices.getVkDevice(0), m_colorFormat, m_depthFormat);

		{
			m_FrameBuffers.resize(m_swapchain.count());
			for (size_t i = 0; i < m_FrameBuffers.size(); ++i)
			{
				m_FrameBuffers[i].create(
					m_devices.getVkDevice(0),
					getWidth(),
					getHeight(),
					m_swapchain.getVkImageView(i),
					m_depthImage.getVkImageView(),
					m_renderPass.getVkRenderPass());
			}
		}

	}

	void initShader()
	{
		//read VertexShader
		std::vector<uint8_t> code(0);
		if (!readBinaryFile(m_vertexShaderName, code))
		{
			std::cout << "could not read vertex shader!!" << std::endl;
		}

		m_vertexShader.create(m_devices.getVkDevice(0), code.size(), code.data());

		//read FragShader
		code.clear();
		if (!readBinaryFile(m_fragShaderName, code))
		{
			std::cout << "could not read fragment shader!!" << std::endl;
		}

		m_fragmentShader.create(m_devices.getVkDevice(0), code.size(), code.data());
	}

	void initBuffer()
	{

		m_vertexBindings.resize(1);
		//init VertexBinding
		m_vertexBindings[0].binding(0);
		m_vertexBindings[0].inputRate(vk::VertexInputRate::eVertex);
		m_vertexBindings[0].stride(sizeof(float) * 2);

		m_vertexAttributes.resize(1);
		//init VertexAttributes
		m_vertexAttributes[0].binding(0);
		m_vertexAttributes[0].location(0);
		m_vertexAttributes[0].format(vk::Format::eR32G32Sfloat);
		m_vertexAttributes[0].offset(0);

		//create VertexBuffer
		m_vertexBuffer.create(m_devices.getVkPhysicalDevice(0), m_devices.getVkDevice(0), vk::BufferUsageFlagBits::eVertexBuffer, sizeof(m_vertexes));

		//write VertexBuffer
		m_vertexBuffer.write(reinterpret_cast<const void*>(m_vertexes));

		//create FragmentBuffer
		m_matrixBuffer.create(m_devices.getVkPhysicalDevice(0), m_devices.getVkDevice(0), vk::BufferUsageFlagBits::eUniformBuffer, sizeof(m_matrix));

		//write FragmentBuffer
		m_matrixBuffer.write(reinterpret_cast<const void*>(m_matrix));

	}

	void initDescriptor()
	{
		//init DescriptorSetLayoutBinding
		std::vector<vk::DescriptorSetLayoutBinding> layoutBinding(1);
		layoutBinding[0].binding(0);
		layoutBinding[0].descriptorType(vk::DescriptorType::eUniformBuffer);
		layoutBinding[0].stageFlags(vk::ShaderStageFlagBits::eVertex);
		layoutBinding[0].pImmutableSamplers(nullptr);
		layoutBinding[0].descriptorCount(1);

		m_descriptorSets.create(m_devices.getVkDevice(0), layoutBinding, 1);

		//write DescriptorSet
		{

			//init DescriptorBufferInfo
			vk::DescriptorBufferInfo bufferInfo;
			bufferInfo.buffer(m_matrixBuffer.getVkBuffer());
			bufferInfo.offset(0);
			bufferInfo.range(sizeof(m_matrix));

			//init WriteDescriptorSet
			vk::WriteDescriptorSet writeSet;
			writeSet.dstSet(m_descriptorSets.getVkDescriptorSet(0));
			writeSet.dstBinding(0);
			writeSet.dstArrayElement(0);
			writeSet.descriptorCount(1);
			writeSet.descriptorType(vk::DescriptorType::eUniformBuffer);
			writeSet.pBufferInfo(&bufferInfo);

			//update DescriptorSet
			m_devices.getVkDevice(0).updateDescriptorSets(1, &writeSet, 0, nullptr);
		}

	}

	void initPipeline()
	{
		//init Viewport
		m_viewport.x(0.0f);
		m_viewport.y(0.0f);
		m_viewport.width((float)getWidth()/2);
		m_viewport.height((float)getHeight());
		m_viewport.minDepth(0.0f);
		m_viewport.maxDepth(1.0f);

		//init Viewport2
		m_viewport2.x((float)getWidth()/2);
		m_viewport2.y(0.0f);
		m_viewport2.width((float)getWidth()/2);
		m_viewport2.height((float)getHeight());
		m_viewport2.minDepth(0.0f);
		m_viewport2.maxDepth(1.0f);

		//init Scissor
		m_scissor.offset().x(0);
		m_scissor.offset().y(0);
		m_scissor.extent().width(getWidth()/2);
		m_scissor.extent().height(getHeight());

		//init Scissor2
		m_scissor2.offset().x(getWidth()/2);
		m_scissor2.offset().y(0);
		m_scissor2.extent().width(getWidth()/2);
		m_scissor2.extent().height(getHeight());

		//create Pipeline
		m_pipeline.create(
			m_devices.getVkDevice(0),
			m_vertexShader.getVkShaderModule(),
			m_fragmentShader.getVkShaderModule(),
			m_descriptorSets.getVkDescriptorSetLayout(),
			m_vertexBindings,
			m_vertexAttributes,
			m_viewport,
			m_scissor,
			m_renderPass.getVkRenderPass());

		//create Pipeline
		m_pipeline2.create(
			m_devices.getVkDevice(0),
			m_vertexShader.getVkShaderModule(),
			m_fragmentShader.getVkShaderModule(),
			m_descriptorSets.getVkDescriptorSetLayout(),
			m_vertexBindings,
			m_vertexAttributes,
			m_viewport2,
			m_scissor2,
			m_renderPass.getVkRenderPass());
	}

	virtual bool initialize()
	{

		m_vertexes[0][0] =  0.0f;
		m_vertexes[0][1] =  1.0f;
		m_vertexes[1][0] =  0.86f;
		m_vertexes[1][1] = -0.5f;
		m_vertexes[2][0] = -0.86f;
		m_vertexes[2][1] = -0.5f;

		setInterval(std::chrono::milliseconds(10));
		vk::Result result;

		m_instance = getInstance();
		
		m_devices.create(m_instance.getVkInstance(), getValidateFlag());

		m_queue.init(m_devices.getVkDevice(0));

		m_cmdBuf.create(m_devices.getVkDevice(0),2);
		

		initFramebuffer();

		initShader();

		initBuffer();

		initDescriptor();

		initPipeline();

		{
			//init SemaphoreCreateInfo
			vk::SemaphoreCreateInfo semaphoreInfo;
			semaphoreInfo.flags(vk::SemaphoreCreateFlagBits());

			//create Semaphore
			result = m_devices.getVkDevice(0).createSemaphore(
                    &semaphoreInfo,
                    nullptr,
                    &m_semaphore);
		}

		//acquire next Image
		result = m_devices.getVkDevice(0).acquireNextImageKHR(
                m_swapchain.getVkSwapchainKHR(),
                400000000, 
                m_semaphore,
                nullptr,
                &m_bufferIndex);

		return true;

	}
	virtual void finalize() override
	{
	}

	virtual bool render() override
	{
		vk::Result result;

		float rate = 100 / (3.14f * 2.0f);
		m_matrix[0][0] = std::cos(m_count / rate);
		m_matrix[0][1] = std::sin(m_count / rate);
		m_matrix[1][0] = -std::sin(m_count / rate);
		m_matrix[1][1] = std::cos(m_count / rate);
		m_matrixBuffer.write(reinterpret_cast<void*>(&m_matrix));

		{
			//init CommandBufferInheritanceInfo
			vk::CommandBufferInheritanceInfo inheritanceInfo;
			inheritanceInfo.renderPass(m_renderPass.getVkRenderPass()); //debug
			inheritanceInfo.subpass(0); //debug
			inheritanceInfo.framebuffer(m_FrameBuffers[m_bufferIndex].getVkFrameBuffer()); //debug
			inheritanceInfo.occlusionQueryEnable(false);
			inheritanceInfo.queryFlags(vk::QueryControlFlagBits());
			inheritanceInfo.pipelineStatistics(vk::QueryPipelineStatisticFlagBits());

			//init CommandBufferBeginInfo
			vk::CommandBufferBeginInfo beginInfo;
			beginInfo.pInheritanceInfo(&inheritanceInfo);

			//begin CommandBuffer
			m_cmdBuf.getVkCommandBuffer(1).begin(&beginInfo);

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
				barrier.subresourceRange().baseArrayLayer(0);
				barrier.subresourceRange().levelCount(1);
				barrier.image(m_swapchain.getVkImage(m_bufferIndex));
				m_cmdBuf.getVkCommandBuffer(1).pipelineBarrier(
					vk::PipelineStageFlagBits::eAllCommands,
					vk::PipelineStageFlagBits::eTopOfPipe,
					vk::DependencyFlagBits(),
					0,
					nullptr,
					0,
					nullptr,
					1,
					&barrier);
			}

		}

		{
			//clear color
			vk::ClearColorValue clearColor(std::array<float, 4>{0.0f, 1.0f, 0.0f, 1.0f});
			vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
			m_cmdBuf.getVkCommandBuffer(1).clearColorImage(m_swapchain.getVkImage(m_bufferIndex), vk::ImageLayout::eGeneral, &clearColor, 1, &range);
		}

		{
			//clear depth
			vk::ClearDepthStencilValue clearDepth(1.0f, 0);
			vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1);
			m_cmdBuf.getVkCommandBuffer(1).clearDepthStencilImage(m_depthImage.getVkImage(), vk::ImageLayout::eGeneral, &clearDepth, 1, &range);
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
			passInfo.renderPass(m_renderPass.getVkRenderPass());
			passInfo.framebuffer(m_FrameBuffers[m_bufferIndex].getVkFrameBuffer());
			passInfo.renderArea().offset().x(0);
			passInfo.renderArea().offset().y(0);
			passInfo.renderArea().extent().width(getWidth());
			passInfo.renderArea().extent().height(getHeight());
			passInfo.clearValueCount(2);
			passInfo.pClearValues(clearValues);

			//begin RenderPass
			m_cmdBuf.getVkCommandBuffer(1).beginRenderPass(&passInfo, vk::SubpassContents::eInline);
		}

		//bind Pipeline
		m_cmdBuf.getVkCommandBuffer(1).bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.getVkPipeline());

		//bind DescriptorSet
		m_cmdBuf.getVkCommandBuffer(1).bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipeline.getVkPipelineLayout(), 0, 1, &m_descriptorSets.getVkDescriptorSet(0), 0, nullptr);

		//set Viewport
		//m_cmdBuf.getVkCommandBuffer(1).setViewport(0, 1, &m_viewport);

		//set Scissor
		//m_cmdBuf.getVkCommandBuffer(1).setScissor(0, 1, &m_scissor);

		//bind VertexBuffer
		vk::DeviceSize offset(0);
		m_cmdBuf.getVkCommandBuffer(1).bindVertexBuffers(0, 1, &m_vertexBuffer.getVkBuffer(), &offset);

		//draw
		m_cmdBuf.getVkCommandBuffer(1).draw(3, 1, 0, 0);

		//bind Pipeline
		m_cmdBuf.getVkCommandBuffer(1).bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline2.getVkPipeline());

		//draw
		m_cmdBuf.getVkCommandBuffer(1).draw(3, 1, 0, 0);

		//end RenderPass
		m_cmdBuf.getVkCommandBuffer(1).endRenderPass();

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
			barrier.subresourceRange().baseArrayLayer(0);
			barrier.subresourceRange().levelCount(1);
			barrier.image(m_swapchain.getVkImage(m_bufferIndex));
			m_cmdBuf.getVkCommandBuffer(1).pipelineBarrier(
				vk::PipelineStageFlagBits::eColorAttachmentOutput,
				vk::PipelineStageFlagBits::eBottomOfPipe,
				vk::DependencyFlagBits(),
				0, nullptr,
				0, nullptr,
				1, &barrier);
		}

		//end CommandBuffer
		m_cmdBuf.getVkCommandBuffer(1).end();

		//submit Queue
		m_queue.submit(m_cmdBuf.getVkCommandBuffer(1), { m_semaphore });

		//wait Queue
		m_queue.wait();

		m_queue.present(m_swapchain.getVkSwapchainKHR(), m_bufferIndex);

		//acquire next Image
		result = m_devices.getVkDevice(0).acquireNextImageKHR(
                m_swapchain.getVkSwapchainKHR(),
                400000000, 
                m_semaphore,
                nullptr,
                &m_bufferIndex);

		//wait Device
		//m_devices.getVkDevice(0).waitIdle();

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
	simpleVulkan::Application* app = new TriangleApplication();
	if (app->create("test", 800, 400, validate))
	{
		app->run();
		app->destroy();
	}
}
