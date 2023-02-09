#pragma once
#include <RGL/Types.hpp>
#include <RGL/Texture.hpp>
#include "RGLD3D12.hpp"
#include <d3d12.h>
#include <RGL/Span.hpp>
#include <D3D12MemAlloc.h>

namespace RGL {
	struct DeviceD3D12;
	struct TextureD3D12 : public ITexture {
		friend class SwapchainD3D12;

		ComPtr<ID3D12Resource> texture;
		ComPtr<ID3D12DescriptorHeap> owningDescriptorHeap = nullptr;
		const UINT descriptorHeapOffset = 0;
		const std::shared_ptr<DeviceD3D12> owningDevice;
		D3D12MA::Allocation* allocation = nullptr;

		TextureD3D12(decltype(texture) image, const Dimension& size, decltype(owningDescriptorHeap), decltype(descriptorHeapOffset), decltype(owningDevice));
		TextureD3D12(decltype(owningDevice), const TextureConfig&, untyped_span bytes);
		TextureD3D12(decltype(owningDevice), const TextureConfig&);


		Dimension GetSize() const final;
		virtual ~TextureD3D12();
	};
}