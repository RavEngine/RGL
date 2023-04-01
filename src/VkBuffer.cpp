#if RGL_VK_AVAILABLE
#include "VkBuffer.hpp"
#include "VkDevice.hpp"
#include "RGLVk.hpp"
#include <cstring>

namespace RGL {

	BufferVk::BufferVk(decltype(owningDevice) owningDevice, const BufferConfig& config) : owningDevice(owningDevice) {
        
        VkMemoryPropertyFlags memprop = 0;
        switch (config.access) {
        case decltype(config.access)::Private:
            memprop = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case decltype(config.access)::Shared:
            memprop = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;
        default:
            FatalError("Unsupported access");
        }

        auto usage = static_cast<VkBufferUsageFlags>(config.type);
        if ((config.options & RGL::BufferFlags::TransferDestination) != RGL::BufferFlags::None) {
            usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        allocation = createBuffer(owningDevice.get(), config.size_bytes, usage, memprop, buffer);

        mappedMemory.size = config.size_bytes;
        stride = config.stride;
	}

    BufferVk::~BufferVk() {
        if (mappedMemory.data != nullptr) {
            UnmapMemory();
        }
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

    void* BufferVk::GetMappedDataPtr()
    {
        return mappedMemory.data;
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