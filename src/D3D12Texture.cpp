#if RGL_DX12_AVAILABLE
#include "D3D12Texture.hpp"
namespace RGL {
	TextureD3D12::TextureD3D12(decltype(texture) image, const Dimension& size, decltype(owningDescriptorHeap) heap, decltype(descriptorHeapOffset) offset, decltype(owningDevice) device) : texture(image), ITexture(size), owningDescriptorHeap(heap), descriptorHeapOffset(offset), owningDevice(device)
	{
	}
	Dimension TextureD3D12::GetSize() const
	{
		return size;
	}
}
#endif