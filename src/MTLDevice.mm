#if RGL_MTL_AVAILABLE

#include "MTLDevice.hpp"

std::shared_ptr<RGL::IDevice> RGL::CreateDefaultDeviceMTL(){
	auto device = MTLCreateSystemDefaultDevice();
	return std::make_shared<DeviceMTL>(device);
}

std::string RGL::DeviceMTL::GetBrandString() {
	
	auto name = [device name];
	return std::string([name UTF8String]);
}

#endif
