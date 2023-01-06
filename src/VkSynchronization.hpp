#pragma once
#include "Synchronization.hpp"
#include <vulkan/vulkan.h>
#include <memory>

namespace RGL {
	struct DeviceVk;

	struct FenceVk : public IFence {
		VkFence fence = VK_NULL_HANDLE;
		const std::shared_ptr<DeviceVk> owningDevice;
		FenceVk(decltype(owningDevice) device, bool preSignaled);
		void Wait() final;
		void Reset() final;
		void Signal() final;
		virtual ~FenceVk();
	};

	struct SemaphoreVk : public ISemaphore {
		VkSemaphore semaphore = VK_NULL_HANDLE;
		const std::shared_ptr<DeviceVk> owningDevice;
		SemaphoreVk(decltype(owningDevice) device);
		virtual ~SemaphoreVk();
	};
}