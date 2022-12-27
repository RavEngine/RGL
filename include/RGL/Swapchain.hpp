#pragma once

namespace RGL{
	struct ISwapchain{
		virtual ~ISwapchain() {}
		virtual void Resize(int width, int height) = 0;
	};
}
