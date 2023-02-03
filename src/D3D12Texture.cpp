#if RGL_DX12_AVAILABLE
#include "D3D12Texture.hpp"
#include <directx/d3dx12.h>
#include "D3D12Device.hpp"
#include "D3D12CommandQueue.hpp"
#include <D3D12MemAlloc.h>

namespace RGL {

	TextureD3D12::TextureD3D12(decltype(texture) image, const Dimension& size, decltype(owningDescriptorHeap) heap, decltype(descriptorHeapOffset) offset, decltype(owningDevice) device) : texture(image), ITexture(size), owningDescriptorHeap(heap), descriptorHeapOffset(offset), owningDevice(device)
	{
	}
	TextureD3D12::TextureD3D12(decltype(owningDevice) owningDevice, const TextureConfig& config, untyped_span bytes) : owningDevice(owningDevice), ITexture({config.width,config.height})
	{
		auto format = DXGI_FORMAT_R8G8B8A8_UINT;	//TODO: obey format

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = config.width;
		resourceDesc.Height = config.depth;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = config.mipLevels;
		resourceDesc.Format = format;		
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		// allocate the resource
		ID3D12Resource* resource;
		D3D12MA::Allocation* allocation;
		HRESULT hr = owningDevice->allocator->CreateResource(
			&allocDesc, &resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST, NULL,
			&allocation, IID_PPV_ARGS(&resource));

		auto commandList = owningDevice->internalQueue->CreateCommandList();


		D3D12MA::ALLOCATION_DESC textureUploadAllocDesc = {};
		textureUploadAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC textureUploadResourceDesc = {};
		textureUploadResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		textureUploadResourceDesc.Alignment = 0;
		textureUploadResourceDesc.Width = bytes.size();
		textureUploadResourceDesc.Height = 1;
		textureUploadResourceDesc.DepthOrArraySize = 1;
		textureUploadResourceDesc.MipLevels = 1;
		textureUploadResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		textureUploadResourceDesc.SampleDesc.Count = 1;
		textureUploadResourceDesc.SampleDesc.Quality = 0;
		textureUploadResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		textureUploadResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		ComPtr<ID3D12Resource> textureUpload;
		D3D12MA::Allocation* textureUploadAllocation;
		DX_CHECK(owningDevice->allocator->CreateResource(
			&textureUploadAllocDesc,
			&textureUploadResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, // pOptimizedClearValue
			&textureUploadAllocation,
			IID_PPV_ARGS(&textureUpload)));
		textureUpload->SetName(L"textureUpload");

		auto bytesPerRow = bytes.size() / config.width;;
		D3D12_SUBRESOURCE_DATA textureSubresourceData = {};
		textureSubresourceData.pData = bytes.data();
		textureSubresourceData.RowPitch = bytesPerRow;
		textureSubresourceData.SlicePitch = bytesPerRow * config.height;

		UpdateSubresources(commandList.Get(), resource, textureUpload.Get(), 0, 0, 1, &textureSubresourceData);


		D3D12_RESOURCE_BARRIER textureBarrier = {};
		textureBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		textureBarrier.Transition.pResource = resource;
		textureBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		textureBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		textureBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &textureBarrier);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = config.mipLevels;

		owningDescriptorHeap = CreateDescriptorHeap(owningDevice->device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,1);

		D3D12_CPU_DESCRIPTOR_HANDLE descHandle = {
				owningDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
		};
		owningDevice->device->CreateShaderResourceView(resource, &srvDesc, descHandle);

		commandList->Close();
		auto fenceValue = owningDevice->internalQueue->ExecuteCommandList(commandList);
		owningDevice->internalQueue->WaitForFenceValue(fenceValue);

	}
	Dimension TextureD3D12::GetSize() const
	{
		return size;
	}
}
#endif