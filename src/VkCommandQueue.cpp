#ifdef RGL_VK_AVAILABLE
#include "VkCommandQueue.hpp"
#include "VkCommandBuffer.hpp"

namespace RGL {
	std::shared_ptr<ICommandBuffer> CommandQueueVk::CreateCommandBuffer()
	{
		return std::make_shared<CommandBufferVk>(shared_from_this());
	}
}

#endif