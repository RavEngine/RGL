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
		
        std::shared_ptr<ISwapchain> CreateSwapchain(std::shared_ptr<ISurface>, std::shared_ptr<ICommandQueue> presentQueue, int, int) final;
        std::shared_ptr<IRenderPass> CreateRenderPass(const RenderPassConfig&) final;

        std::shared_ptr<IPipelineLayout> CreatePipelineLayout(const PipelineLayoutDescriptor&) final;
        std::shared_ptr<IRenderPipeline> CreateRenderPipeline(const RenderPipelineDescriptor&) final;

        std::shared_ptr<IShaderLibrary> CreateShaderLibraryFromName(const std::string_view& name) final;
        std::shared_ptr<IShaderLibrary> CreateDefaultShaderLibrary() final;
        std::shared_ptr<IShaderLibrary> CreateShaderLibraryFromBytes(const std::span<uint8_t>) final;
        std::shared_ptr<IShaderLibrary> CreateShaderLibrarySourceCode(const std::string_view) final;
        std::shared_ptr<IShaderLibrary> CreateShaderLibraryFromPath(const std::filesystem::path&) final;

        std::shared_ptr<IBuffer> CreateBuffer(const BufferConfig&) final;
        std::shared_ptr<ITexture> CreateTextureWithData(const TextureConfig&, untyped_span) final;
        std::shared_ptr<ISampler> CreateSampler(const SamplerConfig&) final;

        std::shared_ptr<ICommandQueue> CreateCommandQueue(QueueType type) final;

        std::shared_ptr<IFence> CreateFence(bool preSignaled) final;
        std::shared_ptr<ISemaphore> CreateSemaphore() final;
        void BlockUntilIdle() final;
        
        virtual ~DeviceMTL(){}
	};

	std::shared_ptr<IDevice> CreateDefaultDeviceMTL();
}

#endif
