#include"queue.h"

#include"device.h"

namespace simpleVulkan
{
    Queue::Queue(){}
    Queue::~Queue(){}

    void Queue::init(vk::Device device)
    {
        m_device = device;
        m_device.getQueue(0,0,&m_queue);
    }

    Result Queue::submit(
		vk::CommandBuffer cmdBuffer, 
		const std::vector<vk::Semaphore>& waitSemaphores, 
		const std::vector<vk::Semaphore>& signalSemaphores)
    {
		vk::PipelineStageFlags stage = vk::PipelineStageFlagBits::eBottomOfPipe;
        vk::Result result;
        //init SubmitInfo
        vk::SubmitInfo submitInfo;
        submitInfo.waitSemaphoreCount(waitSemaphores.size());
		submitInfo.pWaitSemaphores(waitSemaphores.data());
        submitInfo.signalSemaphoreCount(signalSemaphores.size());
        submitInfo.pSignalSemaphores(signalSemaphores.data());
        submitInfo.pWaitDstStageMask(&stage);
        submitInfo.commandBufferCount(1);
        submitInfo.pCommandBuffers(&cmdBuffer);

        //submit queue
        result = m_queue.submit(1,&submitInfo,vk::Fence());
        return result;
    }
    Result Queue::wait()
    {
#ifdef VKCPP_ENHANCED_MODE
		m_queue.waitIdle();
		return Result::eSuccess;
#else
		vk::Result result;
		result = m_queue.waitIdle();
        return result;
#endif
    }
    Result Queue::present(vk::SwapchainKHR swapchain,uint32_t index)
    {
        vk::Result result;
        //init PresentInfoKHR
        vk::PresentInfoKHR presentInfo;
        presentInfo.swapchainCount(1);
        presentInfo.pSwapchains(&swapchain);
        presentInfo.pImageIndices(&index);
        presentInfo.waitSemaphoreCount(0);
        presentInfo.pWaitSemaphores(nullptr);
        presentInfo.pResults(nullptr);

        //present
        result = m_queue.presentKHR(&presentInfo);
        return result;
    }

    vk::Queue& Queue::getVkQueue()
    {
        return m_queue;
    }
}
