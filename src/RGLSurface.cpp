#include "Surface.hpp"
#include "RGL.hpp"
#include "RGLCommon.hpp"

#if RGL_VK_AVAILABLE
#include "VkSurface.hpp"
#endif

#if RGL_DX12_AVAILABLE
#include "D3D12Surface.hpp"
#endif

using namespace RGL;

std::shared_ptr<ISurface> RGL::CreateSurfaceFromPlatformHandle(void* pointer)
{
    switch (CurrentAPI()) {
    case API::Metal:
        FatalError("Not implemented");
        break;
#if RGL_VK_AVAILABLE
    case API::Vulkan:
        return CreateVKSurfaceFromPlatformData(pointer);
#endif
#if RGL_DX12_AVAILABLE
    case API::Direct3D12:
        return CreateD3D12SurfaceFromPlatformData(pointer);
#endif
    default:
        FatalError("Invalid API");
    }

    return std::shared_ptr<ISurface>();
}
