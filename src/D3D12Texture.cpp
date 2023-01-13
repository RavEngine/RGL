#if RGL_DX12_AVAILABLE
#include "D3D12Texture.hpp"
namespace RGL {
	TextureD3D12::TextureD3D12(decltype(texture) image, const Dimension& size) : texture(image), ITexture(size)
	{
	}
	Dimension TextureD3D12::GetSize() const
	{
		return size;
	}
}
#endif