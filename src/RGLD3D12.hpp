#pragma once
#include "RGL.hpp"
#include "RGLCommon.hpp"
#include <cassert>
#include <comdef.h>
#include <wrl.h>
#include <format>

inline void DX_CHECK(HRESULT dx_check_hr) {
	RGL::Assert(!FAILED(dx_check_hr), _com_error(dx_check_hr,nullptr).ErrorMessage());
}
//#define DX_CHECK(hr) {}

using namespace Microsoft::WRL;

namespace RGL {
	void InitD3D12(const RGL::InitOptions&);
	void DeintD3D12();
}