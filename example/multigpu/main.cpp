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
	size_t gpuIndex = 0;
	const std::string m_vertexShaderName = "./vert.spv";
	const std::string m_fragShaderName = "./frag.spv";

	simpleVulkan::Instance m_instance;
	vk::SurfaceKHR m_windowSurface;
	simpleVulkan::Devices m_devices;

	struct
	{
		simpleVulkan::Queue queue;
		simpleVulkan::CommandBuffers cmdBuf;
		vk::Semaphore semaphore;
		simpleVulkan::Swapchain swapchain;
		simpleVulkan::Image depthImage;
		simpleVulkan::RenderPass renderPass;
		std::vector<simpleVulkan::Framebuffer> FrameBuffers;
		simpleVulkan::Buffer vertexBuffer;
		simpleVulkan::Buffer matrixBuffer;
		simpleVulkan::DescriptorSets descriptorSets;
		simpleVulkan::Shader vertexShader;
		simpleVulkan::Shader fragmentShader;
		simpleVulkan::GraphicsPipeline pipeline;
		simpleVulkan::GraphicsPipeline pipeline2;

	} m_gpu[2];

	//Values
	vk::Format m_colorFormat;
	vk::Format m_depthFormat = vk::Format::eD24UnormS8Uint;
	std::vector<vk::VertexInputBindingDescription> m_vertexBindings;
	std::vector<vk::VertexInputAttributeDescription> m_vertexAttributes;
	vk::Viewport m_viewport;
	vk::Viewport m_viewport2;
	vk::Rect2D m_scissor;
	vk::Rect2D m_scissor2;

	//RuntimeVariable
	float m_vertexes[3][2];
	float m_matrix[2][4];
	uint32_t m_bufferIndex;
	uint32_t m_count;


public:
	using simpleVulkan::Application::Application;
private:

	void initFramebuffer()
	{

		m_gpu[gpuIndex].cmdBuf.begin(0);

		m_gpu[gpuIndex].swapchain.create(m_devices.getVkPhysicalDevice(gpuIndex), m_devices.getVkDevice(gpuIndex), m_instance.getSurface(), vk::ImageUsageFlagBits::eColorAttachment, getWidth(), getHeight());
		m_colorFormat = m_gpu[gpuIndex].swapchain.getFormat();

		for (int i = 0; i < m_gpu[gpuIndex].swapchain.count(); ++i)
		{
			//init ImageMemoryBarrier
			vk::ImageMemoryBarrier barrier;
			barrier.srcAccessMask(vk::AccessFlagBits());
			barrier.dstAccessMask(vk::AccessFlagBits::eMemoryRead);
			barrier.oldLayout(vk::ImageLayout::eUndefined);
			barrier.newLayout(vk::ImageLayout::ePresentSrcKHR);
			barrier.image(m_gpu[gpuIndex].swapchain.getVkImage(i));
			barrier.subresourceRange().aspectMask(vk::ImageAspectFlagBits::eColor);
			barrier.subresourceRange().baseMipLevel(0);
			barrier.subresourceRange().levelCount(1);
			barrier.subresourceRange().baseArrayLayer(0);
			barrier.subresourceRange().layerCount(1);

			//layout SwapchainImage
			m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(0).pipelineBarrier(
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

		m_gpu[gpuIndex].depthImage.create(
			m_devices.getVkDevice(gpuIndex),
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
		barrier.image(m_gpu[gpuIndex].depthImage.getVkImage());
		barrier.subresourceRange().aspectMask(vk::ImageAspectFlagBits::eDepth);
		barrier.subresourceRange().baseMipLevel(0);
		barrier.subresourceRange().levelCount(1);
		barrier.subresourceRange().baseArrayLayer(0);
		barrier.subresourceRange().layerCount(1);

		//layout DepthImage
		m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(0).pipelineBarrier(

			vk::PipelineStageFlagBits::eTopOfPipe,
			vk::PipelineStageFlagBits::eTopOfPipe,
			vk::DependencyFlagBits(),
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier);


		m_gpu[gpuIndex].cmdBuf.end(0);

		//init SubmitInfo
		m_gpu[gpuIndex].queue.submit(m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(0));

		//wait queue
		m_gpu[gpuIndex].queue.wait();

		m_gpu[gpuIndex].renderPass.create(m_devices.getVkDevice(gpuIndex), m_colorFormat, m_depthFormat);

		{
			m_gpu[gpuIndex].FrameBuffers.resize(m_gpu[gpuIndex].swapchain.count());
			for (size_t i = 0; i < m_gpu[gpuIndex].FrameBuffers.size(); ++i)
			{
				m_gpu[gpuIndex].FrameBuffers[i].create(
					m_devices.getVkDevice(gpuIndex),
					getWidth(),
					getHeight(),
					m_gpu[gpuIndex].swapchain.getVkImageView(i),
					m_gpu[gpuIndex].depthImage.getVkImageView(),
					m_gpu[gpuIndex].renderPass.getVkRenderPass());
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

		m_gpu[gpuIndex].vertexShader.create(m_devices.getVkDevice(gpuIndex), code.size(), code.data());

		//read FragShader
		code.clear();
		if (!readBinaryFile(m_fragShaderName, code))
		{
			std::cout << "could not read fragment shader!!" << std::endl;
		}

		m_gpu[gpuIndex].fragmentShader.create(m_devices.getVkDevice(gpuIndex), code.size(), code.data());
	}

	void initBuffer()
	{

		//create VertexBuffer
		m_gpu[gpuIndex].vertexBuffer.create(m_devices.getVkPhysicalDevice(gpuIndex), m_devices.getVkDevice(gpuIndex), vk::BufferUsageFlagBits::eVertexBuffer, sizeof(m_vertexes));

		//write VertexBuffer
		m_gpu[gpuIndex].vertexBuffer.write(reinterpret_cast<const void*>(m_vertexes));

		//create FragmentBuffer
		m_gpu[gpuIndex].matrixBuffer.create(m_devices.getVkPhysicalDevice(gpuIndex), m_devices.getVkDevice(gpuIndex), vk::BufferUsageFlagBits::eUniformBuffer, sizeof(m_matrix));

		//write FragmentBuffer
		m_gpu[gpuIndex].matrixBuffer.write(reinterpret_cast<const void*>(m_matrix));

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

		m_gpu[gpuIndex].descriptorSets.create(m_devices.getVkDevice(gpuIndex), layoutBinding, 1);

		//write DescriptorSet
		{

			//init DescriptorBufferInfo
			vk::DescriptorBufferInfo bufferInfo;
			bufferInfo.buffer(m_gpu[gpuIndex].matrixBuffer.getVkBuffer());
			bufferInfo.offset(0);
			bufferInfo.range(sizeof(m_matrix));

			//init WriteDescriptorSet
			vk::WriteDescriptorSet writeSet;
			writeSet.dstSet(m_gpu[gpuIndex].descriptorSets.getVkDescriptorSet(0));
			writeSet.dstBinding(0);
			writeSet.dstArrayElement(0);
			writeSet.descriptorCount(1);
			writeSet.descriptorType(vk::DescriptorType::eUniformBuffer);
			writeSet.pBufferInfo(&bufferInfo);

			//update DescriptorSet
			m_devices.getVkDevice(gpuIndex).updateDescriptorSets(1, &writeSet, 0, nullptr);
		}

	}

	void initPipeline()
	{
		//create Pipeline
		m_gpu[gpuIndex].pipeline.create(
			m_devices.getVkDevice(gpuIndex),
			m_gpu[gpuIndex].vertexShader.getVkShaderModule(),
			m_gpu[gpuIndex].fragmentShader.getVkShaderModule(),
			m_gpu[gpuIndex].descriptorSets.getVkDescriptorSetLayout(),
			m_vertexBindings,
			m_vertexAttributes,
			m_viewport,
			m_scissor,
			m_gpu[gpuIndex].renderPass.getVkRenderPass());

		//create Pipeline
		m_gpu[gpuIndex].pipeline2.create(
			m_devices.getVkDevice(gpuIndex),
			m_gpu[gpuIndex].vertexShader.getVkShaderModule(),
			m_gpu[gpuIndex].fragmentShader.getVkShaderModule(),
			m_gpu[gpuIndex].descriptorSets.getVkDescriptorSetLayout(),
			m_vertexBindings,
			m_vertexAttributes,
			m_viewport2,
			m_scissor2,
			m_gpu[gpuIndex].renderPass.getVkRenderPass());
	}

	virtual bool initialize()
	{

		m_vertexes[0][0] =  0.0f;
		m_vertexes[0][1] =  1.0f;
		m_vertexes[1][0] =  0.86f;
		m_vertexes[1][1] = -0.5f;
		m_vertexes[2][0] = -0.86f;
		m_vertexes[2][1] = -0.5f;

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


		setInterval(std::chrono::milliseconds(10));
		vk::Result result;

		m_instance = getInstance();
		
		m_devices.create(m_instance.getVkInstance(), getValidateFlag());

		m_gpu[gpuIndex].queue.init(m_devices.getVkDevice(gpuIndex));

		m_gpu[gpuIndex].cmdBuf.create(m_devices.getVkDevice(gpuIndex),2);
		

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
			result = m_devices.getVkDevice(gpuIndex).createSemaphore(
                    &semaphoreInfo,
                    nullptr,
                    &m_gpu[gpuIndex].semaphore);
		}

		//acquire next Image
		result = m_devices.getVkDevice(gpuIndex).acquireNextImageKHR(
                m_gpu[gpuIndex].swapchain.getVkSwapchainKHR(),
                400000000, 
                m_gpu[gpuIndex].semaphore,
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
		m_gpu[gpuIndex].matrixBuffer.write(reinterpret_cast<void*>(&m_matrix));

		{
			//init CommandBufferInheritanceInfo
			vk::CommandBufferInheritanceInfo inheritanceInfo;
			inheritanceInfo.renderPass(m_gpu[gpuIndex].renderPass.getVkRenderPass()); //debug
			inheritanceInfo.subpass(0); //debug
			inheritanceInfo.framebuffer(m_gpu[gpuIndex].FrameBuffers[m_bufferIndex].getVkFrameBuffer()); //debug
			inheritanceInfo.occlusionQueryEnable(false);
			inheritanceInfo.queryFlags(vk::QueryControlFlagBits());
			inheritanceInfo.pipelineStatistics(vk::QueryPipelineStatisticFlagBits());

			//init CommandBufferBeginInfo
			vk::CommandBufferBeginInfo beginInfo;
			beginInfo.pInheritanceInfo(&inheritanceInfo);

			//begin CommandBuffer
			m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1).begin(&beginInfo);

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
				barrier.image(m_gpu[gpuIndex].swapchain.getVkImage(m_bufferIndex));
				m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1).pipelineBarrier(
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
			m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1).clearColorImage(m_gpu[gpuIndex].swapchain.getVkImage(m_bufferIndex), vk::ImageLayout::eGeneral, &clearColor, 1, &range);
		}

		{
			//clear depth
			vk::ClearDepthStencilValue clearDepth(1.0f, 0);
			vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1);
			m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1).clearDepthStencilImage(m_gpu[gpuIndex].depthImage.getVkImage(), vk::ImageLayout::eGeneral, &clearDepth, 1, &range);
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
			passInfo.renderPass(m_gpu[gpuIndex].renderPass.getVkRenderPass());
			passInfo.framebuffer(m_gpu[gpuIndex].FrameBuffers[m_bufferIndex].getVkFrameBuffer());
			passInfo.renderArea().offset().x(0);
			passInfo.renderArea().offset().y(0);
			passInfo.renderArea().extent().width(getWidth());
			passInfo.renderArea().extent().height(getHeight());
			passInfo.clearValueCount(2);
			passInfo.pClearValues(clearValues);

			//begin RenderPass
			m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1).beginRenderPass(&passInfo, vk::SubpassContents::eInline);
		}

		//bind Pipeline
		m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1).bindPipeline(vk::PipelineBindPoint::eGraphics, m_gpu[gpuIndex].pipeline.getVkPipeline());

		//bind DescriptorSet
		m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1).bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_gpu[gpuIndex].pipeline.getVkPipelineLayout(), 0, 1, &m_gpu[gpuIndex].descriptorSets.getVkDescriptorSet(0), 0, nullptr);

		//set Viewport
		//m_cmdBuf.getVkCommandBuffer(1).setViewport(0, 1, &m_viewport);

		//set Scissor
		//m_cmdBuf.getVkCommandBuffer(1).setScissor(0, 1, &m_scissor);

		//bind VertexBuffer
		vk::DeviceSize offset(0);
		m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1).bindVertexBuffers(0, 1, &m_gpu[gpuIndex].vertexBuffer.getVkBuffer(), &offset);

		//draw
		m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1).draw(3, 1, 0, 0);

		//bind Pipeline
		m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1).bindPipeline(vk::PipelineBindPoint::eGraphics, m_gpu[gpuIndex].pipeline2.getVkPipeline());

		//draw
		m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1).draw(3, 1, 0, 0);

		//end RenderPass
		m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1).endRenderPass();

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
			barrier.image(m_gpu[gpuIndex].swapchain.getVkImage(m_bufferIndex));
			m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1).pipelineBarrier(
				vk::PipelineStageFlagBits::eColorAttachmentOutput,
				vk::PipelineStageFlagBits::eBottomOfPipe,
				vk::DependencyFlagBits(),
				0, nullptr,
				0, nullptr,
				1, &barrier);
		}

		//end CommandBuffer
		m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1).end();

		//submit Queue
		m_gpu[gpuIndex].queue.submit(m_gpu[gpuIndex].cmdBuf.getVkCommandBuffer(1), { m_gpu[gpuIndex].semaphore });

		//wait Queue
		m_gpu[gpuIndex].queue.wait();

		m_gpu[gpuIndex].queue.present(m_gpu[gpuIndex].swapchain.getVkSwapchainKHR(), m_bufferIndex);

		//acquire next Image
		result = m_devices.getVkDevice(gpuIndex).acquireNextImageKHR(
                m_gpu[gpuIndex].swapchain.getVkSwapchainKHR(),
                400000000, 
                m_gpu[gpuIndex].semaphore,
                nullptr,
                &m_bufferIndex);

		//wait Device
		//m_devices.getVkDevice(gpuIndex).waitIdle();

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
