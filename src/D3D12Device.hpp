#pragma once
#include "Device.hpp"
#include "Pipeline.hpp"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace RGL {

	class DeviceD3D12 : public IDevice{
		Microsoft::WRL::ComPtr<ID3D12Device2> device;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter;
	public:
		DeviceD3D12(decltype(adapter) adapter);
		virtual ~DeviceD3D12();

		// IDevice
		std::string GetBrandString();
		std::shared_ptr<ISwapchain> CreateSwapchain(std::shared_ptr<ISurface>, int, int) final;
		std::shared_ptr<IRenderPass> CreateRenderPass(const RenderPassConfig&) final;
		std::shared_ptr<IPipelineLayout> CreatePipelineLayout(const PipelineLayoutDescriptor&) final;
		std::shared_ptr<IRenderPipeline> CreateRenderPipeline(const std::shared_ptr<IPipelineLayout>, const std::shared_ptr<IRenderPass>, const RenderPipelineDescriptor&) final;

		std::shared_ptr<IShaderLibrary> CreateDefaultShaderLibrary() final;
		std::shared_ptr<IShaderLibrary> CreateShaderLibraryFromBytes(const std::span<uint8_t>) final;
		std::shared_ptr<IShaderLibrary> CreateShaderLibrarySourceCode(const std::string_view) final;
		std::shared_ptr<IShaderLibrary> CreateShaderLibraryFromPath(const std::filesystem::path&) final;

		std::shared_ptr<IBuffer> CreateBuffer(const BufferConfig&) final;
	};

	std::shared_ptr<IDevice> CreateDefaultDeviceD3D12();
}