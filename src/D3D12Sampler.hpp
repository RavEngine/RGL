#pragma once
#include <RGL/Types.hpp>
#include <RGL/Sampler.hpp>
#include "RGLD3D12.hpp"
#include <memory>

namespace RGL {
	struct DeviceD3D12;
	struct SamplerD3D12 : public ISampler {
		const std::shared_ptr<DeviceD3D12> owningDevice;
		D3D12_CPU_DESCRIPTOR_HANDLE descHandle;
		ComPtr<ID3D12DescriptorHeap> owningDescriptorHeap = nullptr;
		SamplerD3D12(decltype(owningDevice), const SamplerConfig&);
		virtual ~SamplerD3D12() {}
	};
}