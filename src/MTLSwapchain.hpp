#pragma once
#include "Swapchain.hpp"
#include "MTLSurface.hpp"

namespace RGL{
	struct SwapchainMTL : public ISwapchain{
		std::shared_ptr<SurfaceMTL> surface;
		SwapchainMTL(decltype(surface) surface) : surface(surface){}
		virtual ~SwapchainMTL() {}
	};
}
