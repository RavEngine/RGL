#pragma once
#include "Pipeline.hpp"
#include "VkDevice.hpp"
#include "VkRenderPass.hpp"

namespace RGL {

	struct PipelineLayoutVk : public IPipelineLayout {
		const std::shared_ptr<DeviceVk> owningDevice;
		VkPipelineLayout layout = VK_NULL_HANDLE;

		PipelineLayoutVk(decltype(owningDevice), const PipelineLayoutDescriptor&);
		virtual ~PipelineLayoutVk();
	};

	struct RenderPipelineVk : public IRenderPipeline {
		const std::shared_ptr<DeviceVk> owningDevice;
		const std::shared_ptr<RenderPassVk> renderPass;
		const std::shared_ptr<PipelineLayoutVk> pipelineLayout;
		VkPipeline graphicsPipeline = VK_NULL_HANDLE;

		RenderPipelineVk(decltype(owningDevice), decltype(renderPass), decltype(pipelineLayout), const RenderPipelineDescriptor&);
		virtual ~RenderPipelineVk();
	};

	
}