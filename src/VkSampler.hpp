#pragma once
#include "Sampler.hpp"
#include <vulkan/vulkan.h>
#include <memory>
namespace RGL {
	struct DeviceVk;
	struct SamplerVk : public ISampler {
		const std::shared_ptr<DeviceVk> owningDevice;
		VkSampler sampler = VK_NULL_HANDLE;
		SamplerVk(decltype(owningDevice), const SamplerConfig& config);
		virtual ~SamplerVk();
	};
}