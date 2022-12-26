#include "RGLVk.hpp"
#include "RGLCommon.hpp"

using namespace RGL;

void RGL::InitVk() {
	Assert(CanInitAPI(RGL::API::Vulkan), "Vulkan cannot be initialized on this platform.");
	RGLGlobals::currentAPI = API::Vulkan;
}