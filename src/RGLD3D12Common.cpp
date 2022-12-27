#include "RGLD3D12.hpp"
#include "RGLCommon.hpp"

using namespace RGL;

void RGL::InitD3D12(const RGL::InitOptions& options) {
	Assert(CanInitAPI(RGL::API::Direct3D12),"Direct3D12 cannot be initialized on this platform.");
	RGL::currentAPI = API::Direct3D12;
}

void RGL::DeintD3D12()
{
	// as of now, do nothing
}

