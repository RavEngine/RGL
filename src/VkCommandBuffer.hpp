#pragma once
#include "CommandBuffer.hpp"
#include <vulkan/vulkan.h>
#include "RGLVk.hpp"

namespace RGL {
	struct DeviceVk;
	struct CommandQueueVk;

	struct CommandBufferVk : public ICommandBuffer {
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;	// does not need to be destroyed
		
		const std::shared_ptr<CommandQueueVk> owningQueue;

		CommandBufferVk(decltype(owningQueue) owningQueue);

		~CommandBufferVk();

		// ICommandBuffer
		void Reset() final;
		void Begin() final;
		void End() final;
		void Commit() final;
	};
}