#if RGL_DX12_AVAILABLE
#include "D3D12Sampler.hpp"
#include "RGLD3D12.hpp"
#include "D3D12Device.hpp"
#include <limits>
#undef max

namespace RGL {
	SamplerD3D12::SamplerD3D12(decltype(owningDevice) owningDevice, const SamplerConfig& config) : owningDevice(owningDevice)
	{
		owningDescriptorHeap = CreateDescriptorHeap(owningDevice->device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1);
		descHandle = {
				owningDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
		};

		D3D12_SAMPLER_DESC samplerDesc{
			.Filter = D3D12_FILTER_ANISOTROPIC,
			.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			.MipLODBias = 0,
			.MaxAnisotropy = 1,
			.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS,
			.BorderColor = {0,0,0,1},
			.MinLOD = 0,
			.MaxLOD = std::numeric_limits<decltype(samplerDesc.MaxLOD)>::max(),

		};

		owningDevice->device->CreateSampler(&samplerDesc, descHandle);
	}
}
#endif