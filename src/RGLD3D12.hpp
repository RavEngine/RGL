#pragma once
#include "RGL.hpp"
#include "RGLCommon.hpp"
#include <cassert>
#include <comdef.h>
#include <wrl.h>
#define DX_CHECK(hr) {auto dx_check_hr = hr; Assert(!FAILED(hr), _com_error(hr).ErrorMessage());}

using namespace Microsoft::WRL;

namespace RGL {
	void InitD3D12(const RGL::InitOptions&);
	void DeintD3D12();
}