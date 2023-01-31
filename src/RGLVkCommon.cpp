#if RGL_VK_AVAILABLE
#include "RGLVk.hpp"
#include "RGLCommon.hpp"
#include "TextureFormat.hpp"
#include <stdexcept>
#include <format>

namespace RGL {

    STATIC(RGL::instance) = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

    // vulkan calls this on debug message
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        constexpr auto vktoDebugSeverity = [](VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
            if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
                return MessageSeverity::Fatal;
            }
            else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
                return MessageSeverity::Warning;
            }
            else {
                return MessageSeverity::Info;
            }
        };

        auto severity = vktoDebugSeverity(messageSeverity);


        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            //TODO: pass through more message types if the user wants them
            LogMessage(severity, pCallbackData->pMessage);
        }

        return VK_FALSE;
    }

    // helper to create the debug messenger
    // by looking up the memory address of the extension function
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    // unload the debug messenger created above
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }


    void RGL::InitVk(const InitOptions& init) {
        Assert(CanInitAPI(RGL::API::Vulkan), "Vulkan cannot be initialized on this platform.");
        RGL::currentAPI = API::Vulkan;

        // create the vulkan instance
        VkApplicationInfo appInfo{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = init.appName.c_str(),
            .applicationVersion = VK_MAKE_API_VERSION(init.appVersion.variant, init.appVersion.major, init.appVersion.minor, init.appVersion.patch),
            .pEngineName = init.engineName.c_str(),
            .engineVersion = VK_MAKE_API_VERSION(init.engineVersion.variant, init.engineVersion.major, init.engineVersion.minor, init.engineVersion.patch),
            .apiVersion = VK_API_VERSION_1_3
        };
        VkInstanceCreateInfo instanceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = 0,
            .enabledExtensionCount = 0
        };

        // validation layers
        std::vector<VkLayerProperties> availableLayers;
        if constexpr (enableValidationLayers) {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
            availableLayers.resize(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (const char* layerName : validationLayers) {

                if (std::find_if(availableLayers.begin(), availableLayers.end(), [layerName](auto& layerProperties) {
                    return strcmp(layerName, layerProperties.layerName) == 0;
                    }
                ) == availableLayers.end()) {
                    throw std::runtime_error(std::format("required validation layer {} not found", layerName));
                }
            }
            instanceCreateInfo.enabledLayerCount = ARRAYSIZE(validationLayers);
            instanceCreateInfo.ppEnabledLayerNames = validationLayers;
        }

        // load GLFW's specific extensions for Vulkan
        const char* minExtensions[] = {
            "VK_KHR_surface",
            "VK_KHR_win32_surface",      // TODO: what is the surface for Linux?
        };
        instanceCreateInfo.enabledExtensionCount = ARRAYSIZE(minExtensions);
        std::vector<const char*> extensions(minExtensions, minExtensions + instanceCreateInfo.enabledExtensionCount);
        if constexpr (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME); // debug callback
        }
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
        instanceCreateInfo.enabledExtensionCount = extensions.size();
        // when doing own implementation, use vkEnumerateInstanceExtensionProperties
        // https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Instance


        // init vulkan
        VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

        // setup the debug messenger
        if constexpr (enableValidationLayers) {
            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{
               .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
               .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
               .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
               .pfnUserCallback = debugCallback,
               .pUserData = nullptr   // optional
            };
            VK_CHECK(CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger));
        }
    }

    void RGL::DeinitVk() {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    SwapChainSupportDetails RGL::querySwapChainSupport(const VkPhysicalDevice device, const VkSurfaceKHR surface) {
        // inquire surface capabilities
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        // get the formats that are supported
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        // get the present modes
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }
    VkSampleCountFlagBits RGL::RGLMSA2VK(const RGL::MSASampleCount& sc)
    {
        auto samplecount = sc;
        {
            switch (samplecount) {
            case decltype(samplecount)::C1: return VK_SAMPLE_COUNT_1_BIT;
            case decltype(samplecount)::C2: return VK_SAMPLE_COUNT_2_BIT;
            case decltype(samplecount)::C4: return VK_SAMPLE_COUNT_4_BIT;
            case decltype(samplecount)::C8: return VK_SAMPLE_COUNT_8_BIT;
            case decltype(samplecount)::C16: return VK_SAMPLE_COUNT_16_BIT;
            case decltype(samplecount)::C32: return VK_SAMPLE_COUNT_32_BIT;
            case decltype(samplecount)::C64: return VK_SAMPLE_COUNT_64_BIT;
            }
        }
    }
    
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

    void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        VkBufferCreateInfo bufferInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer));

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice)
        };


        VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory))

            vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }

    VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue graphicsQueue, VkDevice device, VkCommandPool commandPool) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkQueue graphicsQueue, VkDevice device, VkCommandPool commandPool) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer, graphicsQueue, device, commandPool);
    }
}

#endif
