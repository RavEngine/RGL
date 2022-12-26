#pragma once
#include "Device.hpp"

namespace RGL {

	struct DeviceD3D12 : public IDevice{

	};

	std::shared_ptr<IDevice> CreateDefaultDeviceD3D12();
}