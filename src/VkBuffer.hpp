#pragma once
#include <RGL/Types.hpp>
#include <RGL/Buffer.hpp>
#include <memory>
#include <span>
#include <vulkan/vulkan.h>

namespace RGL {
	struct DeviceVk;

	struct BufferVk : public IBuffer {
		const std::shared_ptr<DeviceVk> owningDevice;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory bufferMemory = VK_NULL_HANDLE;

		MutableSpan mappedMemory;

		BufferVk(decltype(owningDevice), const BufferConfig&);
		virtual ~BufferVk();

		void MapMemory() final;
		void UnmapMemory() final;
		void UpdateBufferData(untyped_span newData) final;
		void SetBufferData(untyped_span data) final;
	};
}