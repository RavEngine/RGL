#if RGL_VK_AVAILABLE
#include "VkSwapchain.hpp"
#include "RGLVk.hpp"
#include <algorithm>

RGL::SwapchainVK::~SwapchainVK(){
    DestroySwapchainIfNeeded();
}

RGL::SwapchainVK::SwapchainVK(decltype(owningSurface) surface, decltype(owningDevice) owningDevice, int width, int height) : owningSurface(surface), owningDevice(owningDevice)
{
    Resize(width, height);
}

void RGL::SwapchainVK::Resize(int width, int height)
{
    // kill the old one
    DestroySwapchainIfNeeded();

    constexpr auto chooseSwapSurfaceFormat = [](const std::vector<VkSurfaceFormatKHR>& availableFormats) -> VkSurfaceFormatKHR {
        // we want BGRA8 SRGB in nonlinear space
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        //otherwise hope the first one is good enough
        return availableFormats[0];
    };
    constexpr auto chooseSwapPresentMode = [](const std::vector<VkPresentModeKHR>& availablePresentModes) -> VkPresentModeKHR {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;    // use Mailbox on high-perf devices
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;        // otherwise use FIFO when on low-power devices, like a mobile phone
    };
    auto chooseSwapExtent = [width,height](const VkSurfaceCapabilitiesKHR& capabilities) ->VkExtent2D {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    };

    // configure the swap chain stuff
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(owningDevice->physicalDevice,owningSurface->surface);
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;  // we want one extra image than necessary to reduce latency (no waiting for the driver)
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    VkSwapchainCreateInfoKHR swapchainCreateInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = owningSurface->surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,      // always 1 unless we are doing stereoscopic 3D
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,   // use VK_IMAGE_USAGE_TRANSFER_DST_BIT for offscreen rendering
        .preTransform = swapChainSupport.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,     // we don't care about pixels that are obscured
        .oldSwapchain = VK_NULL_HANDLE  // future issue
    };
    auto& indices = owningDevice->indices;
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
    if (indices.graphicsFamily != indices.presentFamily) {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0; // Optional
        swapchainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    VK_CHECK(vkCreateSwapchainKHR(owningDevice->device, &swapchainCreateInfo, nullptr, &swapChain));
}

void RGL::SwapchainVK::DestroySwapchainIfNeeded()
{
    if (swapChain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(owningDevice->device, swapChain, nullptr);
    }
}

#endif