#if RGL_DX12_AVAILABLE
#include "D3D12Surface.hpp"

using namespace RGL;

std::shared_ptr<ISurface> RGL::CreateD3D12SurfaceFromPlatformData(void* HWNDptr)
{
	auto hwnd = *static_cast<HWND*>(HWNDptr);
	return std::make_shared<SurfaceD3D12>(hwnd);
}

RGL::SurfaceD3D12::~SurfaceD3D12()
{
	// nothing to do here
}

#endif