#if RGL_VK_AVAILABLE
#include "VkBuffer.hpp"
#include "VkDevice.hpp"
#include "RGLVk.hpp"
#include <cstring>

namespace RGL {

	BufferVk::BufferVk(decltype(owningDevice) owningDevice, const BufferConfig& config) : owningDevice(owningDevice) {
        //TODO: use vkmemoryallocator

        createBuffer(owningDevice->device, owningDevice->physicalDevice, config.size_bytes, static_cast<VkBufferUsageFlags>(config.type), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer, bufferMemory);

        mappedMemory.size = config.size_bytes;
	}

    BufferVk::~BufferVk() {
        vkDestroyBuffer(owningDevice->device, buffer, nullptr);
        vkFreeMemory(owningDevice->device, bufferMemory, nullptr);
    }

    void BufferVk::SetBufferData(untyped_span data) {
        UpdateBufferData(data);
        UnmapMemory();
    }

    void BufferVk::MapMemory() {
        vkMapMemory(owningDevice->device, bufferMemory, 0, mappedMemory.size, 0, &mappedMemory.data);
    }
    void BufferVk::UnmapMemory() {
        vkUnmapMemory(owningDevice->device, bufferMemory);
        mappedMemory.data = nullptr;
    }

    void BufferVk::UpdateBufferData(untyped_span data) {
        if (!mappedMemory.data) {
            MapMemory();
        }
        Assert(data.size() <= mappedMemory.size, "Attempting to write more data than the buffer can hold");
        memcpy(mappedMemory.data, data.data(), data.size());
    }
}

#endif