#pragma once
#include "CommandQueue.hpp"
#include <vulkan/vulkan.h>

namespace RGL {
	struct DeviceVk;
	struct ICommandBuffer;
	
	struct CommandQueueVk : public ICommandQueue, public std::enable_shared_from_this<CommandQueueVk> {
		const std::shared_ptr<DeviceVk> owningDevice;
		CommandQueueVk(decltype(owningDevice) device) : owningDevice(device) {}
		
		std::shared_ptr<ICommandBuffer> CreateCommandBuffer() final;
	};

}