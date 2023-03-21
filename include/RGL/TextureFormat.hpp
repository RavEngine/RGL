#pragma once
#include <cstdint>

namespace RGL {
    enum class ResourceLayout : uint8_t {
        Undefined = 0,
        General = 1,
        ColorAttachmentOptimal = 2,
        DepthStencilAttachmentOptimal = 3,
        DepthStencilReadOnlyOptimal = 4,
        ShaderReadOnlyOptimal = 5,
        TransferSourceOptimal = 6,
        TransferDestinationOptimal,
        Reinitialized,
        DepthReadOnlyStencilAttachmentOptimal,
        DepthAttachmentStencilReadOnlyOptimal,
        DepthAttachmentOptimal,
        DepthReadOnlyOptimal,
        StencilAttachmentOptimal,
        StencilReadOnlyOptimal,
        ReadOnlyOptimal,
        AttachmentOptimal,
        Present
    };


	enum class TextureFormat {
		Undefined,
		BGRA8_Unorm,
        RGBA8_Uint,
        RGBA8_Unorm,
        RGBA16_Unorm,
        RGBA32_Float,

		D32SFloat,			// 32 bit float
		D24UnormS8Uint,		// 24 bit depth, 8 bit stencil
	};

	enum class TextureLayout : uint8_t {
		Present,
		ColorAttachmentOptimal
	};

	enum class MSASampleCount : uint8_t {
            C0 = 0,
			C1 = 1,
			C2 = 2,
			C4 = 4,
			C8 = 8,
			C16 = 16,
			C32 = 32,
            C64 = 64,
	};

	enum class LoadAccessOperation : uint8_t {
		Load = 0,
		Clear,
		DontCare,
		NotAccessed
	};

	enum class StoreAccessOperation : uint8_t {
		Store,
		DontCare,
		None,
	};

    enum TextureUsage {        // matching VkImageUsageFlagBits
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
}
