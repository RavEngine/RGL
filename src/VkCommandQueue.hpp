#pragma once
#include "CommandQueue.hpp"
#include "VkSynchronization.hpp"
#include <vulkan/vulkan.h>
#include <span>

namespace RGL {
	struct DeviceVk;
	struct ICommandBuffer;
	struct CommandBufferVk;
	struct CommitConfig;
	
	struct CommandQueueVk : public ICommandQueue, public std::enable_shared_from_this<CommandQueueVk> {
		const std::shared_ptr<DeviceVk> owningDevice;
		VkQueue queue;
		CommandQueueVk(decltype(owningDevice) device);

		// call by commandbuffer::commit
		void Submit(CommandBufferVk*, const CommitConfig&);
		
		// ICommandQueue
		std::shared_ptr<ICommandBuffer> CreateCommandBuffer() final;

		void WaitUntilCompleted() final;
	};

}