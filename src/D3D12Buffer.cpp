#if RGL_DX12_AVAILABLE
#include "D3D12Buffer.hpp"
#include "D3D12Device.hpp"
#include "D3D12CommandQueue.hpp"

namespace RGL {
    void UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, ID3D12Resource** pDestinationResource, ID3D12Resource** pIntermediateResource, size_t bufferSize, const void* bufferData, Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE)
    {
        // create a resource large enough to hold the data
        auto v = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto t = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);
        DX_CHECK(device->CreateCommittedResource(
            &v,
            D3D12_HEAP_FLAG_NONE,
            &t,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(pDestinationResource)));

        // Create an committed resource for the upload.
        if (bufferData)
        {
            auto t = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            auto v = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
            DX_CHECK(device->CreateCommittedResource(
                &t,
                D3D12_HEAP_FLAG_NONE,
                &v,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(pIntermediateResource)));

            // next transfer the data to the GPU
            D3D12_SUBRESOURCE_DATA subresourceData = {};
            subresourceData.pData = bufferData;
            subresourceData.RowPitch = bufferSize;
            subresourceData.SlicePitch = subresourceData.RowPitch;

            UpdateSubresources(commandList.Get(), *pDestinationResource, *pIntermediateResource, 0, 0, 1, &subresourceData);
        }
    }

	BufferD3D12::BufferD3D12(decltype(owningDevice) device, const BufferConfig& config) : owningDevice(device), myType(config.type)
	{
        switch (config.type) {
        case decltype(config.type)::VertexBuffer:
            vertexBufferView.SizeInBytes = config.size_bytes;
            vertexBufferView.StrideInBytes = config.stride;
            break;
        case decltype(config.type)::IndexBuffer:
            indexBufferView.SizeInBytes = config.size_bytes;
            indexBufferView.Format = decltype(indexBufferView.Format)::DXGI_FORMAT_R32_UINT;   //TODO: support 16-bit index buffer
            break;
        default:
            FatalError("current buffer type is not supported");
        };
		
	}
	void BufferD3D12::MapMemory()
	{
	}

	void BufferD3D12::UnmapMemory()
	{
	}
	void BufferD3D12::UpdateBufferData(untyped_span newData)
	{
        //TODO: don't do this every time
		ComPtr<ID3D12Resource> intermediateVertexBuffer;
        auto commandList = owningDevice->internalQueue->CreateCommandList();
		UpdateBufferResource(commandList.Get(), &buffer, &intermediateVertexBuffer, newData.size(), newData.data(), owningDevice->device);
        vertexBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
        indexBufferView.BufferLocation = vertexBufferView.BufferLocation;   //NOTE: if this is made a union, check this

        commandList->Close();
        auto fenceValue = owningDevice->internalQueue->ExecuteCommandList(commandList);
        owningDevice->internalQueue->WaitForFenceValue(fenceValue);
	}
	void BufferD3D12::SetBufferData(untyped_span data)
	{
        UpdateBufferData(data);
	}
}
#endif


