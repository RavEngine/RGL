#include "RGLD3D12.hpp"
#include "RGLCommon.hpp"

using namespace RGL;

void RGL::InitD3D12() {
	Assert(CanInitAPI(RGL::API::Direct3D12),"Direct3D12 cannot be initialized on this platform.");
	RGLGlobals::currentAPI = API::Direct3D12;
}