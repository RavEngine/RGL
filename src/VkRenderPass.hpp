#pragma once
#include <RGL/Types.hpp>
#include <RGL/Pipeline.hpp>
#include <vulkan/vulkan.h>
#include "VkDevice.hpp"

namespace RGL {
	struct RenderPassVk : public IRenderPass {
		const std::shared_ptr<DeviceVk> device;
		VkRenderPass renderPass = VK_NULL_HANDLE;
		VkFramebuffer passFrameBuffer = VK_NULL_HANDLE;
		uint32_t currentWidth = 0, currentHeight = 0;
		std::vector<VkFormat> attachmentFormats;

		RenderPassVk(decltype(device), const RenderPassConfig&);
		virtual ~RenderPassVk();

		/**
		Updates the internal framebuffer if needed. 
		@param width requested width of the framebuffer
		@param height requested height of the framebuffer
		*/
		void UpdateFramebuffer(decltype(currentWidth) width, decltype(currentHeight) height);
	};

}