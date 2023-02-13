#if RGL_VK_AVAILABLE
#include "VkCommandQueue.hpp"
#include "VkCommandBuffer.hpp"
#include "VkDevice.hpp"
#include "VkSynchronization.hpp"

namespace RGL {
    CommandQueueVk::CommandQueueVk(decltype(owningDevice) device) : owningDevice(device)
    {
        vkGetDeviceQueue(device->device, device->indices.graphicsFamily.value(), 0, &queue);    // 0 because we only have 1 queue
        VK_VALID(queue);
    }
    void CommandQueueVk::Submit(CommandBufferVk* cb, const CommitConfig& config)
	{

        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSubmitInfo submitInfo{
           .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
           .waitSemaphoreCount = 0,
           .pWaitSemaphores = nullptr,
           .pWaitDstStageMask = waitStages,
           .commandBufferCount = 1,
           .pCommandBuffers = &(cb->commandBuffer),
           .signalSemaphoreCount = 0,
           .pSignalSemaphores = nullptr
        };
        auto fence = std::static_pointer_cast<FenceVk>(config.signalFence);
        VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, fence->fence));
	}
    RGLCommandBufferPtr CommandQueueVk::CreateCommandBuffer()
	{
		return std::make_shared<CommandBufferVk>(shared_from_this());
	}
    void CommandQueueVk::WaitUntilCompleted()
    {
        vkQueueWaitIdle(queue);
    }
}

#endif
