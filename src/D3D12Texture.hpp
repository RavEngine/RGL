#pragma once
#include "Texture.hpp"
#include "RGLD3D12.hpp"
#include <d3d12.h>

namespace RGL {
	struct DeviceD3D12;
	struct TextureD3D12 : public ITexture {
		friend class SwapchainD3D12;

		ComPtr<ID3D12Resource> texture;
		ComPtr<ID3D12DescriptorHeap> owningDescriptorHeap = nullptr;
		const UINT descriptorHeapOffset = 0;
		const std::shared_ptr<DeviceD3D12> owningDevice;

		TextureD3D12(decltype(texture) image, const Dimension& size, decltype(owningDescriptorHeap), decltype(descriptorHeapOffset), decltype(owningDevice));

		Dimension GetSize() const final;
		virtual ~TextureD3D12() {};
	};
}