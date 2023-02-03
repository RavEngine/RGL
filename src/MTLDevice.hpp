#pragma once

#if RGL_MTL_AVAILABLE
#include "Device.hpp"
#import <Metal/Metal.h>
#include <memory>

namespace RGL{

	struct DeviceMTL : public IDevice, public std::enable_shared_from_this<DeviceMTL>{
		id<MTLDevice> device = nullptr;
        id<MTLLibrary> defaultLibrary = nullptr;
	
        DeviceMTL(decltype(device) device);
		std::string GetBrandString() final;
		
        std::shared_ptr<ISwapchain> CreateSwapchain(std::shared_ptr<ISurface>, RGLCommandQueuePtr presentQueue, int, int) final;
        std::shared_ptr<IRenderPass> CreateRenderPass(const RenderPassConfig&) final;

        std::shared_ptr<IPipelineLayout> CreatePipelineLayout(const PipelineLayoutDescriptor&) final;
        std::shared_ptr<IRenderPipeline> CreateRenderPipeline(const RenderPipelineDescriptor&) final;

        RGLShaderLibraryPtr CreateShaderLibraryFromName(const std::string_view& name) final;
        RGLShaderLibraryPtr CreateDefaultShaderLibrary() final;
        RGLShaderLibraryPtr CreateShaderLibraryFromBytes(const std::span<uint8_t>) final;
        RGLShaderLibraryPtr CreateShaderLibrarySourceCode(const std::string_view) final;
        RGLShaderLibraryPtr CreateShaderLibraryFromPath(const std::filesystem::path&) final;

        RGLBufferPtr CreateBuffer(const BufferConfig&) final;
        RGLTexturePtr CreateTextureWithData(const TextureConfig&, untyped_span) final;
        RGLSamplerPtr CreateSampler(const SamplerConfig&) final;

        RGLCommandQueuePtr CreateCommandQueue(QueueType type) final;

        RGLFencePtr CreateFence(bool preSignaled) final;
        RGLSemaphorePtr CreateSemaphore() final;
        void BlockUntilIdle() final;
        
        virtual ~DeviceMTL(){}
	};

	std::shared_ptr<IDevice> CreateDefaultDeviceMTL();
}

#endif
