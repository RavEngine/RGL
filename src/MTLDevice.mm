#if RGL_MTL_AVAILABLE

#include "MTLDevice.hpp"
#include "MTLSurface.hpp"
#include "MTLSwapchain.hpp"

std::shared_ptr<RGL::IDevice> RGL::CreateDefaultDeviceMTL(){
	auto device = MTLCreateSystemDefaultDevice();
	return std::make_shared<DeviceMTL>(device);
}

std::string RGL::DeviceMTL::GetBrandString() {
	
	auto name = [device name];
	return std::string([name UTF8String]);
}

std::shared_ptr<RGL::ISwapchain> RGL::DeviceMTL::CreateSwapchain(std::shared_ptr<ISurface> isurface, int, int){
	auto surface = std::static_pointer_cast<RGL::SurfaceMTL>(isurface);
	[surface->layer setDevice:device];
	return std::make_shared<SwapchainMTL>(surface);
}

#endif
