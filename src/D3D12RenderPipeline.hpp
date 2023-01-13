#pragma once
#include "Pipeline.hpp"
#include "RGLD3D12.hpp"
#include <d3d12.h>
#include <directx/d3dx12.h>

namespace RGL {
	struct DeviceD3D12;
	struct PipelineLayoutD3D12 : public IPipelineLayout {
		const std::shared_ptr<DeviceD3D12> owningDevice;
		ComPtr<ID3D12RootSignature> rootSignature;

		PipelineLayoutD3D12(decltype(owningDevice), const PipelineLayoutDescriptor&);
		void SetLayout(const LayoutConfig& config) final;
	};

	struct RenderPipelineD3D12 : public IRenderPipeline {
		const std::shared_ptr<DeviceD3D12> owningDevice;
		ComPtr<ID3D12PipelineState> pipelineState;
		const std::shared_ptr<PipelineLayoutD3D12> pipelineLayout;

		RenderPipelineD3D12(decltype(owningDevice), const RenderPipelineDescriptor&);
	};
}