#if RGL_DX12_AVAILABLE
#include "D3D12DynamicDescriptorHeap.hpp"
#include <DescriptorHeap.h>
#include <cassert>

namespace RGL {
	D3D12DynamicDescriptorHeap::D3D12DynamicDescriptorHeap(decltype(owningDevice) device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags) : owningDevice(device), DescriptorHeap(device,type,flags, totalCount)
	{
		// start with 1 pile
	}
	D3D12DynamicDescriptorHeap::index_t D3D12DynamicDescriptorHeap::AllocateSingle()
	{
		index_t nextIndex = 0;
		if (freeList.empty()) {
			if (end == totalCount-1) {
				// full -- cannot allocate!
				throw std::out_of_range("Descriptor heap is full!");
			}
			// place the descriptor at the end
			nextIndex = end;
			end++;
		}
		else {
			// fill the hole
			nextIndex = freeList.front();
			freeList.pop();
		}

		return nextIndex;
	}

	void D3D12DynamicDescriptorHeap::DeallocateSingle(index_t index)
	{
		// add the index to the free list
		freeList.emplace(index);
		// if the index was the end, then decrement end
		if (index == end) {
			end--;
		}
	}

}
#endif