#pragma once
#include "Device.hpp"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace RGL {

	class DeviceD3D12 : public IDevice{
		Microsoft::WRL::ComPtr<ID3D12Device2> device;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter;
	public:
		DeviceD3D12(decltype(adapter) adapter);
		virtual ~DeviceD3D12();

		std::string GetBrandString();
	};

	std::shared_ptr<IDevice> CreateDefaultDeviceD3D12();
}