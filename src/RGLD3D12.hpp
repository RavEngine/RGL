#pragma once
#include "RGL.hpp"

#include <cassert>
#define DX_CHECK(hr) (assert(!FAILED(hr)))

namespace RGL {
	void InitD3D12(const RGL::InitOptions&);
	void DeintD3D12();
}