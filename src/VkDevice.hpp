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
		VkDevice device;
		VkPhysicalDevice physicalDevice;	// does not need to be destroyed
		QueueFamilyIndices indices;
		VkQueue presentQueue, graphicsQueue;	// do not need to be destroyed
	public:
		virtual ~DeviceVk();
		DeviceVk(
			decltype(device) logicalDevice, 
			decltype(physicalDevice) physicalDevice, 
			const decltype(indices)& queueFamilyIndices,
			decltype(presentQueue) presentQueue, 
			decltype(graphicsQueue) graphicsQueue
		) :
			device(logicalDevice), physicalDevice(physicalDevice), indices(queueFamilyIndices),
			presentQueue(presentQueue), graphicsQueue(graphicsQueue)
		{}

		std::string GetBrandString() final;
		
	};

	std::shared_ptr<IDevice> CreateDefaultDeviceVk();
}