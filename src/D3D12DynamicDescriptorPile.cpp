#if RGL_DX12_AVAILABLE
#include "D3D12DynamicDescriptorPile.hpp"
#include <DescriptorHeap.h>
#include <cassert>

namespace RGL {
	D3D12DynamicDescriptorPile::D3D12DynamicDescriptorPile(decltype(owningDevice) device, decltype(type) type, decltype(flags) flags) : type(type), flags(flags), owningDevice(device)
	{
		// start with 1 pile
		allocateNewPile();
	}
	size_t D3D12DynamicDescriptorPile::AllocateSingle()
	{
		auto& pile = descriptorPiles.at(findFirstNonFullPile());
		return pile->Allocate();

	}
	D3D12DynamicDescriptorPile::AllocationRange D3D12DynamicDescriptorPile::AllocateRange(size_t count)
	{
		auto& pile = descriptorPiles.at(findFirstNonFullPile(0, count));
		
		AllocationRange range{};
		pile->AllocateRange(count, range.begin, range.end);

		return range;
	}
	void D3D12DynamicDescriptorPile::allocateNewPile()
	{
		descriptorPiles.emplace_back(std::move(std::make_unique<DirectX::DescriptorPile>(owningDevice,
			this->type,
			this->flags,
			descriptorsPerPile)));
	}
	size_t D3D12DynamicDescriptorPile::findFirstNonFullPile(size_t start, size_t nRequired)
	{
		if (nRequired > descriptorsPerPile) {
			throw std::runtime_error("Cannot contiguously allocate required number of descriptors");
		}

		assert(start < descriptorPiles.size());
		for (auto i = start; i < descriptorPiles.size(); i++) {
			if (descriptorsPerPile - descriptorPiles[i]->Count() >= nRequired) {
				return i;
			}
		}

		// otherwise, all descriptor piles are full, and we need a new one
		allocateNewPile();
		return descriptorPiles.size() - 1;
	}
}
#endif