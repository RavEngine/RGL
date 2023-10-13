#if RGL_WEBGPU_AVAILABLE
#include "WGSwapchain.hpp"
#include "WGDevice.hpp"
#include <emscripten/html5_webgpu.h>
#include "RGLCommon.hpp"

namespace RGL{
    WGPUSwapChain makeSwapchain(std::shared_ptr<SurfaceWG> surface, uint32_t width, uint32_t height, const std::shared_ptr<DeviceWG> owningDevice){
        WGPUSwapChainDescriptor swapChainDesc{
            .nextInChain = nullptr,
            .width = width,
            .height = height,
            .format = wgpuSurfaceGetPreferredFormat(surface->surface,owningDevice->adapter),
            .usage = WGPUTextureUsage_RenderAttachment,
            .presentMode = WGPUPresentMode_Fifo,
        };
        return wgpuDeviceCreateSwapChain(owningDevice->device, surface->surface, &swapChainDesc);
    }

    SwapchainWG::SwapchainWG(decltype(surface) surface, uint32_t width, uint32_t height, const std::shared_ptr<DeviceWG> owningDevice) : surface(surface), owningDevice(owningDevice){
        swapchain = makeSwapchain(surface, width, height, owningDevice);
    }

    SwapchainWG::~SwapchainWG(){
        wgpuSwapChainRelease(swapchain);
    }

    void SwapchainWG::Resize(uint32_t width, uint32_t height){
        wgpuSwapChainRelease(swapchain);
        swapchain = makeSwapchain(surface, width, height, owningDevice);
    }

    void SwapchainWG::GetNextImage(uint32_t* index){
        *index = 0;
        WGPUTextureView nextTexture = wgpuSwapChainGetCurrentTextureView(swapchain);
        FatalError("GetNextImage: not implemented");
    }

    ITexture* SwapchainWG::ImageAtIndex(uint32_t index){
        FatalError("ImageAtIndex: not implemented");
        return nullptr;
    }

    void SwapchainWG::Present(const SwapchainPresentConfig&){
        wgpuSwapChainPresent(swapchain);
    }
}

#endif