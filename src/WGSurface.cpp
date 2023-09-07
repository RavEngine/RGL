#if RGL_WEBGPU_AVAILABLE
#include "WGSurface.hpp"
#include "RGLWG.hpp"
#include <iostream>

namespace RGL{


    SurfaceWG::SurfaceWG(const void* pointer){
        // we are expecting a CSS selector string, like "#canvas", for the value of pointer
        WGPUSurfaceDescriptor desc{
            .nextInChain = nullptr,
            .label = static_cast<const char* const>(pointer)
        };
        surface = wgpuInstanceCreateSurface(instance, &desc);
    }

    SurfaceWG::~SurfaceWG(){
        wgpuSurfaceRelease(surface);
    }


    RGLSurfacePtr CreateWGSurfaceFromPlatformHandle(const void* pointer){
        return std::make_shared<SurfaceWG>(pointer);
    }
}
#endif