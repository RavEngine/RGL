#pragma once
#include "Device.hpp"
#include <optional>
#include <vulkan/vulkan.h>

namespace RGL {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	class DeviceVk : public IDevice {
		VkDevice device = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;	// does not need to be destroyed
		QueueFamilyIndices indices;
		VkQueue presentQueue = VK_NULL_HANDLE, graphicsQueue = VK_NULL_HANDLE;	// do not need to be destroyed
	public:
		virtual ~DeviceVk();
		DeviceVk(decltype(physicalDevice) physicalDevice);

		std::string GetBrandString() final;
		
	};

	std::shared_ptr<IDevice> CreateDefaultDeviceVk();
}