#pragma once
#include "Texture.hpp"
#include "RGLD3D12.hpp"
#include <d3d12.h>

namespace RGL {
	struct TextureD3D12 : public ITexture {
		friend class SwapchainD3D12;

		ComPtr<ID3D12Resource> texture;
		TextureD3D12(decltype(texture) image, const Dimension& size);

		Dimension GetSize() const final;
		virtual ~TextureD3D12() {};
	};
}