#pragma once
#include <memory>
#include <string>
#include "Swapchain.hpp"
#include "Surface.hpp"
#include "Pipeline.hpp"

namespace RGL {

	struct IDevice {
		virtual ~IDevice() {}
		static std::shared_ptr<IDevice> CreateSystemDefaultDevice();

		virtual std::string GetBrandString() = 0;
		
		virtual std::shared_ptr<ISwapchain> CreateSwapchain(std::shared_ptr<ISurface>, int width, int height) = 0;
		virtual std::shared_ptr<IRenderPass> CreateRenderPass(const RenderPassConfig&) = 0;

		virtual std::shared_ptr<IPipelineLayout> CreatePipelineLayout(const PipelineLayoutDescriptor&) = 0;
		virtual std::shared_ptr<IRenderPipeline> CreateRenderPipeline(const std::shared_ptr<IPipelineLayout>, const std::shared_ptr<IRenderPass>, const RenderPipelineDescriptor&) = 0;
	};
}
