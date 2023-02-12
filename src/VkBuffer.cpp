#if RGL_VK_AVAILABLE
#include "VkBuffer.hpp"
#include "VkDevice.hpp"
#include "RGLVk.hpp"
#include <cstring>

namespace RGL {

	BufferVk::BufferVk(decltype(owningDevice) owningDevice, const BufferConfig& config) : owningDevice(owningDevice) {

       allocation =  createBuffer(owningDevice.get(), config.size_bytes, static_cast<VkBufferUsageFlags>(config.type), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer);

        mappedMemory.size = config.size_bytes;
	}

    BufferVk::~BufferVk() {
        vkDestroyBuffer(owningDevice->device, buffer, nullptr);
        vmaFreeMemory(owningDevice->vkallocator, allocation);
    }

    void BufferVk::SetBufferData(untyped_span data, decltype(BufferConfig::size_bytes) offset) {
        UpdateBufferData(data, offset);
        UnmapMemory();
    }

    decltype(BufferConfig::size_bytes) BufferVk::getBufferSize() const
    {
        return mappedMemory.size;
    }

    void BufferVk::MapMemory() {
        vmaMapMemory(owningDevice->vkallocator, allocation, &mappedMemory.data);
    }
    void BufferVk::UnmapMemory() {
        vmaUnmapMemory(owningDevice->vkallocator, allocation);
        mappedMemory.data = nullptr;
    }

    void BufferVk::UpdateBufferData(untyped_span data, decltype(BufferConfig::size_bytes) offset) {
        if (!mappedMemory.data) {
            MapMemory();
        }
        Assert(data.size() + offset <= mappedMemory.size, "Attempting to write more data than the buffer can hold");
        memcpy(static_cast<std::byte*>(mappedMemory.data) + offset, data.data(), data.size());
    }
}

#endif