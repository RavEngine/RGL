#include "Surface.hpp"
#include "RGL.hpp"
#include "RGLCommon.hpp"
#include "VkSurface.hpp"
#include "D3D12Surface.hpp"

using namespace RGL;

std::shared_ptr<ISurface> RGL::CreateSurfaceFromPlatformHandle(void* pointer)
{
    switch (CurrentAPI()) {
    case API::Metal:
        FatalError("Not implemented");
        break;
    case API::Vulkan:
        return CreateVKSurfaceFromPlatformData(pointer);
    case API::Direct3D12:
        return CreateD3D12SurfaceFromPlatformData(pointer);
    default:
        FatalError("Invalid API");
    }

    return std::shared_ptr<ISurface>();
}
