# pragma once

#include "RGLCommon.hpp"
#include <vulkan/vulkan.h>
#include <cassert>

#define VK_CHECK(a) {auto VK_CHECK_RESULT = a; assert(VK_CHECK_RESULT == VK_SUCCESS);}
#define VK_VALID(a) {assert(a != VK_NULL_HANDLE);}

constexpr bool enableValidationLayers =
#ifdef NDEBUG
false;
#else
true;
#endif

constexpr static const char* const validationLayers[] = {
	"VK_LAYER_KHRONOS_validation"
};


namespace RGL {
	void InitVk(const InitOptions&);
	void DeinitVk();

	extern VkInstance instance;

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice device, const VkSurfaceKHR surface);

	enum class MSASampleCount : uint8_t;
	VkSampleCountFlagBits RGLMSA2VK(const RGL::MSASampleCount& samplecount);
}