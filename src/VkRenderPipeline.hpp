#pragma once
#include <RGL/Types.hpp>
#include <RGL/Pipeline.hpp>
#include "VkDevice.hpp"
#include "VkRenderPass.hpp"

namespace RGL {

	struct PipelineLayoutVk : public IPipelineLayout {
		const std::shared_ptr<DeviceVk> owningDevice;
		VkPipelineLayout layout = VK_NULL_HANDLE;
		VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

		PipelineLayoutVk(decltype(owningDevice), const PipelineLayoutDescriptor&);
		virtual ~PipelineLayoutVk();

		// IPipelineLayout
		void SetLayout(const LayoutConfig& config) final;
	};

	struct RenderPipelineVk : public IRenderPipeline {
		const std::shared_ptr<DeviceVk> owningDevice;
		const std::shared_ptr<PipelineLayoutVk> pipelineLayout;
		VkPipeline graphicsPipeline = VK_NULL_HANDLE;

		RenderPipelineVk(decltype(owningDevice), const RenderPipelineDescriptor&);
		virtual ~RenderPipelineVk();
	};

	
}