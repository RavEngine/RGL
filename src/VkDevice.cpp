#if RGL_VK_AVAILABLE
#include "VkDevice.hpp"
#include "RGLVk.hpp"
#include "VkSwapchain.hpp"
#include <vector>
#include <stdexcept>
#include <set>
#include <format>

using namespace RGL;

constexpr static const char* const deviceExtensions[] = {
       VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

bool checkDeviceExtensionSupport(const VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(std::begin(deviceExtensions), std::end(deviceExtensions));

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
};

// find a queue of the right family
QueueFamilyIndices findQueueFamilies (VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    // Logic to find queue family indices to populate struct with

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            indices.presentFamily = i;  // note: to do this properly, one should use vkGetPhysicalDeviceSurfaceSupportKHR and check surface support, but we don't have a surface
            // in general graphics queues are able to present. if the use has different needs, they should not use the default device.
        }

        /*VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }*/
        i++;
    }
    return indices;
};


std::shared_ptr<IDevice> RGL::CreateDefaultDeviceVk() {
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    // now select and configure a device
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    Assert(deviceCount != 0, "No GPUs with Vulkan support");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());


    constexpr auto isDeviceSuitable = [](const VkPhysicalDevice device) -> bool {
        // look for all the features we want
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        auto queueFamilyData = findQueueFamilies(device);

        auto extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            /*SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            bool swapchainSupported = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();*/
            swapChainAdequate = /*swapchainSupported &&*/ swapChainAdequate;
        }

        // right now we don't care so pick any gpu
        // in the future implement a scoring system to pick the best device
        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && queueFamilyData.isComplete() && checkDeviceExtensionSupport(device);
    };
    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }
    Assert(physicalDevice != VK_NULL_HANDLE, "failed to find a suitable GPU!");

	return std::make_shared<DeviceVk>( physicalDevice);
}

RGL::DeviceVk::DeviceVk(decltype(physicalDevice) physicalDevice) : physicalDevice(physicalDevice) {
    // next create the logical device and the queue
    indices = findQueueFamilies(physicalDevice);
    float queuePriority = 1.0f;     // required even if we only have one queue. Used to cooperatively schedule multiple queues

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
        };
        queueCreateInfos.push_back(queueCreateInfo);
    }
    VkPhysicalDeviceFeatures deviceFeatures{};      // we don't yet need anything
    VkDeviceCreateInfo deviceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<decltype(VkDeviceCreateInfo::queueCreateInfoCount)>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),      // could pass an array here if we were making more than one queue
        .enabledExtensionCount = ARRAYSIZE(deviceExtensions),             // device-specific extensions are ignored on later vulkan versions but we set it anyways
        .ppEnabledExtensionNames = deviceExtensions,
        .pEnabledFeatures = &deviceFeatures,
    };
    if constexpr (enableValidationLayers) {
        deviceCreateInfo.enabledLayerCount = ARRAYSIZE(validationLayers);
        deviceCreateInfo.ppEnabledLayerNames = validationLayers;
    }
    VK_CHECK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);    // 0 because we only have 1 queue
    VK_VALID(graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    VK_VALID(presentQueue);
}

RGL::DeviceVk::~DeviceVk() {

    vkDestroyDevice(device, nullptr);
}

std::string RGL::DeviceVk::GetBrandString() {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(physicalDevice, &props);
    return props.deviceName;
}

std::shared_ptr<ISwapchain> RGL::DeviceVk::CreateSwapchain(std::shared_ptr<ISurface> surface, int width, int height)
{

    return std::make_shared<SwapchainVK>(std::static_pointer_cast<RGL::SurfaceVk>(surface), shared_from_this(), width, height);
}

#endif
