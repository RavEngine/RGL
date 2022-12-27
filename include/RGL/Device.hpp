#pragma once
#include <memory>
#include <string>
#include "Swapchain.hpp"
#include "Surface.hpp"

namespace RGL {

	struct IDevice {
		virtual ~IDevice() {}
		static std::shared_ptr<IDevice> CreateSystemDefaultDevice();

		virtual std::string GetBrandString() = 0;
		
		virtual std::shared_ptr<ISwapchain> CreateSwapchain(std::shared_ptr<ISurface>, int width, int height) = 0;
	};
}
