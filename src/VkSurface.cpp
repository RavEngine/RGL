#if RGL_VK_AVAILABLE
#include "VkSurface.hpp"
#include "RGLVk.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif

using namespace RGL;

std::shared_ptr<ISurface> RGL::CreateVKSurfaceFromPlatformData(void* pointer)
{
    VkSurfaceKHR surface;
#ifdef _WIN32
    auto hwnd = *static_cast<HWND*>(pointer);
    VkWin32SurfaceCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
         .hinstance = GetModuleHandle(nullptr),
        .hwnd = hwnd,
    };
    VK_CHECK(vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface));
#elif defined __linux__
#error Surface not implemented - see vkCreateXcbSurfaceKHR for X11/xcb
#endif

    return std::make_shared<SurfaceVk>(surface);
}

SurfaceVk::~SurfaceVk() {
    vkDestroySurfaceKHR(instance, surface, nullptr);
}
#endif
