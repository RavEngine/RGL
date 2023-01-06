#pragma once
#include "Synchronization.hpp"
#include "RGLD3D12.hpp"
#include <d3d12.h>

namespace RGL {
	struct DeviceD3D12;

	// implements a vulkan-style binary fence
	struct FenceD3D12 : public IFence {
		const std::shared_ptr<DeviceD3D12> owningDevice;
		HANDLE fenceEvent;

		FenceD3D12(decltype(owningDevice));

		ComPtr<ID3D12Fence> fence;

		// IFence
		void Wait() final;
		void Reset() final;
		void Signal() final;
		virtual ~FenceD3D12() {}
	};

	// does nothing because DX12 does not have semaphores
	struct SemaphoreD3D12 : public ISemaphore {

	};
}