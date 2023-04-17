#pragma once
#include <RGL/Types.hpp>
#include <RGL/Pipeline.hpp>
#include "RGLD3D12.hpp"
#include <d3d12.h>
#include <directx/d3dx12.h>
#include <vector>
#include <unordered_map>

namespace RGL {
	struct DeviceD3D12;
	struct PipelineLayoutD3D12 : public IPipelineLayout {
		const std::shared_ptr<DeviceD3D12> owningDevice;
		ComPtr<ID3D12RootSignature> rootSignature;
		const PipelineLayoutDescriptor config;
		uint32_t firstBufferIdx = 0, firstSamplerIdx = 0;

		std::unordered_map<uint32_t, uint32_t> 
			bufferBindingToRootSlot,
			samplerBindingtoRootSlot,
			textureBindingToRootSlot;

		auto slotForBufferIdx(uint32_t bindingPos) {
			return bufferBindingToRootSlot.at(bindingPos);
		}

		auto slotForSamplerIdx(uint32_t bindingPos) {
			return samplerBindingtoRootSlot.at(bindingPos);
		}

		auto slotForTextureIdx(uint32_t bindingPos) {
			return textureBindingToRootSlot.at(bindingPos);
		}

		bool bufferIdxIsUAV(uint32_t bindingPos) {
			const auto buffIdx = slotForBufferIdx(bindingPos) - firstSamplerIdx;
			return bufferIsUAV[buffIdx];
		}
		std::vector<bool> bufferIsUAV;

		PipelineLayoutD3D12(decltype(owningDevice), const PipelineLayoutDescriptor&);
	};

	struct RenderPipelineD3D12 : public IRenderPipeline {
		const std::shared_ptr<DeviceD3D12> owningDevice;
		ComPtr<ID3D12PipelineState> pipelineState;
		const std::shared_ptr<PipelineLayoutD3D12> pipelineLayout;

		RenderPipelineD3D12(decltype(owningDevice), const RenderPipelineDescriptor&);
	};
}