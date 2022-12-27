#include "Surface.hpp"
#include "RGL.hpp"
#include "RGLCommon.hpp"
#include "VkSurface.hpp"

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
        FatalError("Not implemented");
        break;
    default:
        FatalError("Invalid API");
    }

    return std::shared_ptr<ISurface>();
}
