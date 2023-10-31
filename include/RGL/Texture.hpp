#pragma once
#include <cstdint>
#include "TextureFormat.hpp"

#if RGL_VK_AVAILABLE
#include <vulkan/vulkan.h>
#endif

namespace RGL {

	struct Dimension {
		uint32_t width = 0, height = 0;
	};
	
	enum class TextureType : uint8_t {
		T1D,
		T2D,
		T3D,
	};

	enum class TilingMode : uint8_t {
		Optimal,
		Linear
	};

struct TextureView{
	const RGL::ITexture* parent = nullptr;

#if RGL_VK_AVAILABLE || RGL_DX12_AVAILABLE
	Dimension viewSize{ 0,0 };
#endif
    union {
#if RGL_MTL_AVAILABLE
        id mtl;
#endif
#if RGL_DX12_AVAILABLE
		
#endif
#if RGL_VK_AVAILABLE
		VkImageView vk;
#endif
    } texture;
    
#if RGL_VK_AVAILABLE
	TextureView(decltype(parent) parent, VkImageView in_img, Dimension dim) : parent(parent), viewSize(dim) {
		texture.vk = in_img;
	}
	TextureView() {
		// intentionally blank
	}
#endif
#if RGL_MTL_AVAILABLE
    TextureView(id tx){
        texture.mtl = tx;
    }
#endif
};

	struct TextureConfig {
		TextureUsage usage;
		TextureAspect aspect;
		uint32_t width = 0, height = 0, depth = 1, mipLevels = 1, arrayLayers = 1;
		TextureType imageType = decltype(imageType)::T2D;
		TextureFormat format;
		TilingMode mode = decltype(mode)::Optimal;
		ResourceLayout initialLayout = ResourceLayout::Undefined;
		const char* debugName = nullptr;
		bool readbackEnabled = false;
	};

	class ITexture {
	protected:
		Dimension size;
	public:
		ITexture(decltype(size) size) : size(size) {}
		virtual Dimension GetSize() const = 0;
        virtual TextureView GetDefaultView() const = 0;
        virtual TextureView GetViewForMip(uint32_t mip) const = 0;
	};

}
