#if RGL_VK_AVAILABLE
#include "RGLVk.hpp"
#include "RGLCommon.hpp"
#include <stdexcept>
#include <format>

using namespace RGL;

STATIC(RGL::instance) = VK_NULL_HANDLE;
VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

// vulkan calls this on debug message
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    constexpr auto vktoDebugSeverity = [](VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
        return MessageSeverity::Info;   //TODO: fix
    };

    auto severity = vktoDebugSeverity(messageSeverity);


    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        //TODO: pass through more message types if the user wants them
        LogMessage(severity, pCallbackData->pMessage);

#ifdef NDEBUG
        __debugbreak();
#endif
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
    const char* glfwExtensions[] = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface"      // TODO: what is the surface for Linux?
    };
    instanceCreateInfo.enabledExtensionCount = ARRAYSIZE(glfwExtensions);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + instanceCreateInfo.enabledExtensionCount);
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
    vkDestroyInstance(instance,nullptr);
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
};

#endif
