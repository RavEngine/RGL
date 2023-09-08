#if RGL_WEBGPU_AVAILABLE
#include "WGSwapchain.hpp"
#include <emscripten/html5_webgpu.h>

namespace RGL{
    SwapchainWG::SwapchainWG(decltype(surface) surface, uint32_t width, uint32_t height, const std::shared_ptr<DeviceWG> owningDevice){
        WGPUSwapChainDescriptor swapChainDesc{
            .nextInChain = nullptr,
            .width = width,
            .height = height,
            .format = wgpuSurfaceGetPreferredFormat(surface->surface,owningDevice->adapter),
            .usage = WGPUTextureUsage_RenderAttachment,
            .presentMode = WGPUPresentMode_Fifo,
        };
        swapchain = wgpuDeviceCreateSwapChain(device, surface->surface, &swapChainDesc)
    }

    SwapchainWG::~SwapchainWG(){
        wgpuSwapchainRelease(swapchain);
    }
}

#endif