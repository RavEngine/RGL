#pragma once

namespace RGL {
	enum class TextureFormat {
		BGRA8_Unorm
	};

	enum class TextureLayout : uint8_t {
		Present,
		ColorAttachmentOptimal
	};

	enum class MSASampleCount : uint8_t {
			C1 = 1,
			C2,
			C4,
			C8,
			C16,
			C32,
			C64
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