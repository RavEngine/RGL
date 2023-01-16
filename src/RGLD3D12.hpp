#pragma once
#include "RGL.hpp"
#include "RGLCommon.hpp"
#include <cassert>
#include <comdef.h>
#include <wrl.h>
#include <format>
#define DX_CHECK(hr) {auto dx_check_hr = hr; Assert(!FAILED(dx_check_hr), std::format("{} - {}", #hr, _com_error(dx_check_hr).ErrorMessage()));}

using namespace Microsoft::WRL;

namespace RGL {
	void InitD3D12(const RGL::InitOptions&);
	void DeintD3D12();
}