#pragma once
#include "Device.hpp"

namespace RGL {

	struct DeviceVk : public IDevice {

	};

	std::shared_ptr<IDevice> CreateDefaultDeviceVk();
}