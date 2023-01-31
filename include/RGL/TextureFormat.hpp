#pragma once

namespace RGL {
	enum class TextureFormat {
		BGRA8_Unorm,
        RGBA8_SFloat,
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
}
