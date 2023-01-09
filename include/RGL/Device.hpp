#pragma once
#include <memory>
#include <string>
#include <span>
#include <filesystem>
#include "CommandQueue.hpp"
#undef CreateSemaphore

namespace RGL {
	struct ISwapchain;
	struct IRenderPass;
	struct ISurface;
	struct IPipelineLayout;
	struct IRenderPipeline;

	struct RenderPassConfig;
	struct RenderPipelineDescriptor;
	struct PipelineLayoutDescriptor;
	struct IShaderLibrary;

	struct BufferConfig;
	struct IBuffer;
	struct IFence;
	struct ISemaphore;

	struct IDevice {
		virtual ~IDevice() {}
		static std::shared_ptr<IDevice> CreateSystemDefaultDevice();

		virtual std::string GetBrandString() = 0;
		
		virtual std::shared_ptr<ISwapchain> CreateSwapchain(std::shared_ptr<ISurface>, int width, int height) = 0;
		virtual std::shared_ptr<IRenderPass> CreateRenderPass(const RenderPassConfig&) = 0;

		virtual std::shared_ptr<IPipelineLayout> CreatePipelineLayout(const PipelineLayoutDescriptor&) = 0;
		virtual std::shared_ptr<IRenderPipeline> CreateRenderPipeline(const RenderPipelineDescriptor&) = 0;

        virtual std::shared_ptr<IShaderLibrary> CreateShaderLibraryFromName(const std::string_view& name) = 0;
		virtual std::shared_ptr<IShaderLibrary> CreateDefaultShaderLibrary() = 0;
		virtual std::shared_ptr<IShaderLibrary> CreateShaderLibraryFromBytes(const std::span<uint8_t>) = 0;
		virtual std::shared_ptr<IShaderLibrary> CreateShaderLibrarySourceCode(const std::string_view) = 0;
		virtual std::shared_ptr<IShaderLibrary> CreateShaderLibraryFromPath(const std::filesystem::path&) = 0;

		virtual std::shared_ptr<IBuffer> CreateBuffer(const BufferConfig&) = 0;

		virtual std::shared_ptr<ICommandQueue> CreateCommandQueue(QueueType type) = 0;

		virtual std::shared_ptr<IFence> CreateFence(bool preSignaled) = 0;
		virtual std::shared_ptr<ISemaphore> CreateSemaphore() = 0;
		virtual void BlockUntilIdle() = 0;
	};
}
