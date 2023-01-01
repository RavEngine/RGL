#pragma once
#include "Texture.hpp"

namespace RGL{
	struct ISwapchain{
		virtual ~ISwapchain() {}
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual ITexture& GetNextImage() = 0;
	};
}
