#pragma once
#include "Texture.hpp"

namespace RGL{
	struct ISwapchain{
		virtual ~ISwapchain() {}
		virtual void Resize(int width, int height) = 0;
		virtual ITexture& GetNextImage() = 0;
	};
}
