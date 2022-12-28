#pragma once
#include "Pipeline.hpp"
#include <vulkan/vulkan.h>
#include "VkDevice.hpp"

namespace RGL {
	struct RenderPassVk : public IRenderPass {
		const std::shared_ptr<DeviceVk> device;
		VkRenderPass renderPass = VK_NULL_HANDLE;

		RenderPassVk(decltype(device), const RenderPassConfig&);
		virtual ~RenderPassVk();

	};

}