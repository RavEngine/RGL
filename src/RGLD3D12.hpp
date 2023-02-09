#pragma once
#include <RGL/Types.hpp>
#include <RGL/Core.hpp>
#include "RGLCommon.hpp"
#include <cassert>
#include <comdef.h>
#include <wrl.h>
#include <format>
#include <d3d12.h>

inline void DX_CHECK(HRESULT dx_check_hr) {
	RGL::Assert(!FAILED(dx_check_hr), _com_error(dx_check_hr,nullptr).ErrorMessage());
}
//#define DX_CHECK(hr) {}

using namespace Microsoft::WRL;

namespace RGL {
	void InitD3D12(const RGL::InitOptions&);
	void DeintD3D12();
	struct RenderPassConfig;

	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device2> device,
		D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);

	RGLRenderPassPtr CreateRenderPassD3D12(const RenderPassConfig& config);
}
