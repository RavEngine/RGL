#pragma once

#include "Surface.hpp"
#include <vulkan/vulkan.h>

namespace RGL {

	struct SurfaceVk : public ISurface {
		VkSurfaceKHR surface;
		SurfaceVk(decltype(surface) surface) : surface(surface) {}
		virtual ~SurfaceVk();
	};

	std::shared_ptr<ISurface> CreateVKSurfaceFromPlatformData(void*);
}