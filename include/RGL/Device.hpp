#pragma once
#include <memory>

namespace RGL {

	struct IDevice {
		virtual ~IDevice() {}

		static std::shared_ptr<IDevice> CreateSystemDefaultDevice();
	};
}