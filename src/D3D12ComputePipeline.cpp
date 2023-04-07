#if RGL_DX12_AVAILABLE
#include "D3D12ComputePipeline.hpp"
#include "D3D12Device.hpp"
#include "RGLD3D12.hpp"
#include "D3D12RenderPipeline.hpp"

namespace RGL {
	ComputePipelineD3D12::ComputePipelineD3D12(const decltype(owningDevice) owningDevice, const ComputePipelineDescriptor& desc) : owningDevice(owningDevice)
	{
		auto device = owningDevice->device;

		D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateDesc{
			.pRootSignature = std::static_pointer_cast<PipelineLayoutD3D12>(desc.pipelineLayout)->rootSignature.Get(),
			.CS = desc.stage.shaderModule.get(),
			.CachedPSO = nullptr,
		};

		DX_CHECK(device->CreateComputePipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState)));
	}
	ComputePipelineD3D12::~ComputePipelineD3D12()
	{

	}
}
#endif