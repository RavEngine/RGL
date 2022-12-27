#pragma once

#if RGL_MTL_AVAILABLE
#include "Device.hpp"
#import <Metal/Metal.h>

namespace RGL{

	class DeviceMTL : public IDevice{
		id<MTLDevice> device = nullptr;
	public:
		DeviceMTL(decltype(device) device) : device(device){}
		std::string GetBrandString() final;
		
		std::shared_ptr<ISwapchain> CreateSwapchain(std::shared_ptr<ISurface>, int, int) final;
	};

	std::shared_ptr<IDevice> CreateDefaultDeviceMTL();
}

#endif
