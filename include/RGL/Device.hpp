#pragma once
#include <memory>
#include <string>
#include <span>
#include <filesystem>
#include "CommandQueue.hpp"
#include "Span.hpp"
#include <RGL/Types.hpp>

#undef CreateSemaphore

namespace RGL {

	struct IDevice {
		virtual ~IDevice() {}
		static RGLDevicePtr CreateSystemDefaultDevice();

		virtual std::string GetBrandString() = 0;
		
		virtual RGLSwapchainPtr CreateSwapchain(std::shared_ptr<ISurface>, RGLCommandQueuePtr presentQueue, int width, int height) = 0;
		virtual RGLRenderPassPtr CreateRenderPass(const RenderPassConfig&) = 0;

		virtual RGLPipelineLayoutPtr CreatePipelineLayout(const PipelineLayoutDescriptor&) = 0;
		virtual RGLRenderPipelinePtr CreateRenderPipeline(const RenderPipelineDescriptor&) = 0;

        virtual RGLShaderLibraryPtr CreateShaderLibraryFromName(const std::string_view& name) = 0;
		virtual RGLShaderLibraryPtr CreateDefaultShaderLibrary() = 0;
		virtual RGLShaderLibraryPtr CreateShaderLibraryFromBytes(const std::span<uint8_t>) = 0;
		virtual RGLShaderLibraryPtr CreateShaderLibrarySourceCode(const std::string_view) = 0;
		virtual RGLShaderLibraryPtr CreateShaderLibraryFromPath(const std::filesystem::path&) = 0;

		virtual RGLBufferPtr CreateBuffer(const BufferConfig&) = 0;
		virtual RGLTexturePtr CreateTextureWithData(const TextureConfig&, untyped_span) = 0;
        virtual RGLSamplerPtr CreateSampler(const SamplerConfig&) = 0;

		virtual RGLCommandQueuePtr CreateCommandQueue(QueueType type) = 0;

		virtual RGLFencePtr CreateFence(bool preSignaled) = 0;
		virtual RGLSemaphorePtr CreateSemaphore() = 0;
		virtual void BlockUntilIdle() = 0;
	};
}
