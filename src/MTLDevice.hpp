#pragma once

#if RGL_MTL_AVAILABLE
#include "Device.hpp"
#import <Metal/Metal.h>

namespace RGL{

	class DeviceMTL : public IDevice{
		id<MTLDevice> device = nullptr;
	public:
		DeviceMTL(decltype(device) device) : device(device){}
		std::string GetBrandString() final;
		
		std::shared_ptr<ISwapchain> CreateSwapchain(std::shared_ptr<ISurface>, int, int) final;
        std::shared_ptr<IRenderPass> CreateRenderPass(const RenderPassConfig&) final;

        std::shared_ptr<IPipelineLayout> CreatePipelineLayout(const PipelineLayoutDescriptor&) final;
        std::shared_ptr<IRenderPipeline> CreateRenderPipeline(const RenderPipelineDescriptor&) final;

        std::shared_ptr<IShaderLibrary> CreateDefaultShaderLibrary() final;
        std::shared_ptr<IShaderLibrary> CreateShaderLibraryFromBytes(const std::span<uint8_t>) final;
        std::shared_ptr<IShaderLibrary> CreateShaderLibrarySourceCode(const std::string_view) final;
        std::shared_ptr<IShaderLibrary> CreateShaderLibraryFromPath(const std::filesystem::path&) final;

        std::shared_ptr<IBuffer> CreateBuffer(const BufferConfig&) final;

        std::shared_ptr<ICommandQueue> CreateCommandQueue(QueueType type) final;

        std::shared_ptr<IFence> CreateFence(bool preSignaled) final;
        std::shared_ptr<ISemaphore> CreateSemaphore() final;
        void BlockUntilIdle() final;
        
        virtual ~DeviceMTL(){}
	};

	std::shared_ptr<IDevice> CreateDefaultDeviceMTL();
}

#endif
