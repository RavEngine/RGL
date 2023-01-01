#pragma once
#include <cstdint>

namespace RGL {

	struct Dimension {
		uint32_t width = 0, height = 0;
	};
	
	class ITexture {
	protected:
		Dimension size;
	public:
		ITexture(decltype(size) size) : size(size) {}
		virtual Dimension GetSize() const = 0;
	};
}