#if RGL_VK_AVAILABLE
#include "VkBuffer.hpp"
#include "VkDevice.hpp"
#include "RGLVk.hpp"

namespace RGL {

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        // find a memory type suitable for the buffer
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            // needs to have the right support
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

	BufferVk::BufferVk(decltype(owningDevice) owningDevice, const BufferConfig& config) : owningDevice(owningDevice) {
        //TODO: use vkmemoryallocator

        auto device = owningDevice->device;
        VkBufferCreateInfo bufferInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = config.size_bytes,
            .usage = static_cast<VkBufferUsageFlags>(config.type),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer));

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memRequirements.size,
            //TODO: make properties configurable
            .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, owningDevice->physicalDevice)
        };


        VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory))

        vkBindBufferMemory(device, buffer, bufferMemory, 0);

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