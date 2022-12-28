#pragma once

namespace RGL {
	enum class TextureFormat {
		BGRA8_Unorm
	};

	enum class TextureLayout {
		Present,
		ColorAttachmentOptimal
	};

	enum class MSASampleCount {
			C1 = 1,
			C2,
			C4,
			C8,
			C16,
			C32,
			C64
	};

	enum class LoadAccessOperation {
		Load = 0,
		Clear,
		DontCare,
		NotAccessed
	};

	enum class StoreAccessOperation {
		Store,
		DontCare,
		None,
	};
}