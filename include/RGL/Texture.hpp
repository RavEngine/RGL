#pragma once
#include <cstdint>
#include "TextureFormat.hpp"

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

	enum TextureUsage {		// matching VkImageUsageFlagBits 
		TransferSource = 0x00000001,
		TransferDestination = 0x00000002,
		Sampled = 0x00000004,
		Storage = 0x00000008,
		ColorAttachment = 0x00000010,
		DepthStencilAttachment = 0x00000020,
		TransientAttachment = 0x00000040,
		InputAttachment = 0x00000080
	};

	enum TextureAspect {
		None = 0,
		HasColor = 0x00000001,
		HasDepth = 0x00000002,
		HasStencil = 0x00000004,
		HasMetadata = 0x00000008,
	};

	inline TextureUsage operator|(TextureUsage a, TextureUsage b)
	{
		return static_cast<TextureUsage>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline TextureAspect operator|(TextureAspect a, TextureAspect b)
	{
		return static_cast<TextureAspect>(static_cast<int>(a) | static_cast<int>(b));
	}

	struct TextureConfig {
		TextureUsage usage;
		TextureAspect aspect = TextureAspect::None;
		uint32_t width = 0, height = 0, depth = 1, mipLevels = 1, arrayLayers = 1;
		TextureType imageType = decltype(imageType)::T2D;
		TextureFormat format;
		TilingMode mode = decltype(mode)::Optimal;
	};

	class ITexture {
	protected:
		Dimension size;
	public:
		ITexture(decltype(size) size) : size(size) {}
		virtual Dimension GetSize() const = 0;
	};
}