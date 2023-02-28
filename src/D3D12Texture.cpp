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
	TextureD3D12::TextureD3D12(decltype(texture) image, const TextureConfig& config, std::shared_ptr<IDevice> indevice) : owningDevice(std::static_pointer_cast<DeviceD3D12>(indevice)), ITexture({config.width, config.height}), texture(image)
	{
		// make the heap and SRV 
		const bool isDS = (config.aspect & TextureAspect::HasDepth || config.aspect & TextureAspect::HasStencil);
		const auto type = isDS ? D3D12_DESCRIPTOR_HEAP_TYPE_DSV : D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		auto format = rgl2dxgiformat_texture(config.format);
		CreateHeapAndSRV(owningDevice, type, false, format, config);

		if (!isDS) {
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(owningDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			D3D12_RENDER_TARGET_VIEW_DESC desc{
				.Format = format,
				.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
			};
			owningDevice->device->CreateRenderTargetView(image.Get(), &desc, rtvHandle);
		}
		
	}
	TextureD3D12::TextureD3D12(decltype(owningDevice) owningDevice, const TextureConfig& config, untyped_span bytes) : TextureD3D12(owningDevice, config)
	{
		auto commandList = owningDevice->internalQueue->CreateCommandList();

		// create the staging buffer
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

		UpdateSubresources(commandList.Get(), texture.Get(), textureUpload.Get(), 0, 0, 1, &textureSubresourceData);


		D3D12_RESOURCE_BARRIER textureBarrier = {};
		textureBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		textureBarrier.Transition.pResource = texture.Get();
		textureBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		textureBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		textureBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &textureBarrier);


		commandList->Close();
		auto fenceValue = owningDevice->internalQueue->ExecuteCommandList(commandList);
		owningDevice->internalQueue->WaitForFenceValue(fenceValue);

		textureUploadAllocation->Release();	// no longer need this so get rid of it

	}
	TextureD3D12::TextureD3D12(decltype(owningDevice) owningDevice, const TextureConfig& config) : owningDevice(owningDevice), ITexture({ config.width,config.height })
	{
		const auto format = rgl2dxgiformat_texture(config.format);

		const bool isDS = (config.aspect & TextureAspect::HasDepth || config.aspect & TextureAspect::HasStencil);

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = config.width;
		resourceDesc.Height = config.height;
		resourceDesc.DepthOrArraySize = config.arrayLayers;
		resourceDesc.MipLevels = config.mipLevels;
		resourceDesc.Format = format;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = isDS ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE;

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		// allocate the resource
		
		const auto state = isDS ? D3D12_RESOURCE_STATE_DEPTH_WRITE : (D3D12_RESOURCE_STATE_COPY_DEST);

		D3D12_CLEAR_VALUE optimizedClearValue = {
			.Format = resourceDesc.Format,
		};

		if (isDS) {
			optimizedClearValue.DepthStencil = { 1,0 };
		}
		else {
			std::fill(optimizedClearValue.Color, optimizedClearValue.Color + std::size(optimizedClearValue.Color), 0);
		}

		HRESULT hr = owningDevice->allocator->CreateResource(
			&allocDesc, &resourceDesc,
			state, (resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL || resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) ? &optimizedClearValue : nullptr,
			&allocation, IID_PPV_ARGS(&texture));

		texture->SetName(L"Texture Resource");

		const D3D12_DESCRIPTOR_HEAP_TYPE type = (isDS ? D3D12_DESCRIPTOR_HEAP_TYPE_DSV : D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		const bool canBeShadervisible = !(resourceDesc.Flags & (D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET));

		CreateHeapAndSRV(owningDevice, type, canBeShadervisible, format, config);

		if (!isDS && config.usage & RGL::TextureUsage::ColorAttachment) {
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(owningDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			D3D12_RENDER_TARGET_VIEW_DESC desc{
				.Format = format,
				.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
			};
			owningDevice->device->CreateRenderTargetView(texture.Get(), &desc, rtvHandle);
		}
	}
	void TextureD3D12::CreateHeapAndSRV(const std::shared_ptr<RGL::DeviceD3D12>& owningDevice, const D3D12_DESCRIPTOR_HEAP_TYPE& type, const bool& canBeShadervisible, const DXGI_FORMAT& format, const RGL::TextureConfig& config)
	{
		owningDescriptorHeap = CreateDescriptorHeap(owningDevice->device, type, 1, canBeShadervisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

		auto descHandle = owningDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		// create the correct type of resource view
		if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) {
			// create the resource view
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = config.mipLevels;


			owningDevice->device->CreateShaderResourceView(texture.Get(), &srvDesc, descHandle);
		}
		else if (type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV) {
			D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
			dsv.Format = rgl2dxgiformat_texture(config.format);
			dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsv.Texture2D.MipSlice = 0;
			dsv.Flags = D3D12_DSV_FLAG_NONE;

			owningDevice->device->CreateDepthStencilView(texture.Get(), &dsv, descHandle);
		}
	}
	Dimension TextureD3D12::GetSize() const
	{
		return size;
	}
	TextureD3D12::~TextureD3D12()
	{
		if (allocation) {
			allocation->Release();
		}
	}
}
#endif