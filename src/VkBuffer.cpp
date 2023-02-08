#if RGL_VK_AVAILABLE
#include "VkBuffer.hpp"
#include "VkDevice.hpp"
#include "RGLVk.hpp"
#include <cstring>

namespace RGL {

	BufferVk::BufferVk(decltype(owningDevice) owningDevice, const BufferConfig& config) : owningDevice(owningDevice) {
        //TODO: use vkmemoryallocator

       allocation =  createBuffer(owningDevice.get(), config.size_bytes, static_cast<VkBufferUsageFlags>(config.type), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer);

        mappedMemory.size = config.size_bytes;
	}

    BufferVk::~BufferVk() {
        vkDestroyBuffer(owningDevice->device, buffer, nullptr);
        vmaFreeMemory(owningDevice->vkallocator, allocation);
    }

    void BufferVk::SetBufferData(untyped_span data) {
        UpdateBufferData(data);
        UnmapMemory();
    }

    void BufferVk::MapMemory() {
        vmaMapMemory(owningDevice->vkallocator, allocation, &mappedMemory.data);
    }
    void BufferVk::UnmapMemory() {
        vmaUnmapMemory(owningDevice->vkallocator, allocation);
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