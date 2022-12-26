# pragma once

#include "RGLCommon.hpp"
#include <vulkan/vulkan.h>
#include <cassert>

#define VK_CHECK(a) {auto VK_CHECK_RESULT = a; assert(VK_CHECK_RESULT == VK_SUCCESS);}
#define VK_VALID(a) {assert(a != VK_NULL_HANDLE);}


namespace RGL {
	void InitVk(const InitOptions&);
	void DeinitVk();

	extern VkInstance instance;
}