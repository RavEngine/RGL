#pragma once
#include <vector>
#include <memory>
#include <d3d12.h>

namespace DirectX {
	struct DescriptorPile;
}


namespace RGL {
	struct D3D12DynamicDescriptorPile {
	private:
		ID3D12Device* owningDevice;
	public:
		const D3D12_DESCRIPTOR_HEAP_TYPE type;
		const D3D12_DESCRIPTOR_HEAP_FLAGS flags;

		D3D12DynamicDescriptorPile(decltype(owningDevice) device, decltype(type) type, decltype(flags) flags);

		/**
		* Allocate a single descriptor. Returns the offset into the descriptor heap.
		*/
		size_t AllocateSingle();

		struct AllocationRange {
			size_t begin = 0, end = 0;
		};
		/**
		* Contiguously allocate a range of descriptors. Returns the start and end of the block.
		* If the block cannot be contiguously allocated, then this method throws. 
		*/
		AllocationRange AllocateRange(size_t count);

	private:
		constexpr static auto descriptorsPerPile = 128u;
		void allocateNewPile();
		
		// creates a new pile if necessary
		size_t findFirstNonFullPile(size_t start = 0, size_t nRequired = 1);

		std::vector<std::unique_ptr<DirectX::DescriptorPile>> descriptorPiles;
	};
}