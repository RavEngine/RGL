#pragma once
#include "Buffer.hpp"
#include "RGLD3D12.hpp"
#include <d3d12.h>
#include <directx/d3dx12.h>
#include <memory>

namespace RGL {
	struct DeviceD3D12;

	struct BufferD3D12 : public IBuffer {
		ComPtr<ID3D12Resource> buffer;
		D3D12_VERTEX_BUFFER_VIEW bufferView;
		const std::shared_ptr<DeviceD3D12> owningDevice;

		BufferD3D12(decltype(owningDevice), const BufferConfig&);

		/**
		* Map system RAM for updating this buffer.
		*/
		void MapMemory() final;

		/**
		* Unmap system RAM for updating this buffer.
		*/
		void UnmapMemory() final;

		/**
		Update the contents of this buffer. If memory is not mapped, it will become mapped. The memory remains mapped. Intended to be used with UniformBuffers or other data that changes frequently.
		@param newData the data to write into the buffer.
		*/
		void UpdateBufferData(untyped_span newData) final;

		/**
		Set the contents of this buffer. Intended to be used with VertexBuffers or other data that changes infrequently or never.
		@param newData the data to write into the buffer.
		*/
		void SetBufferData(untyped_span data) final;


		virtual ~BufferD3D12() {}
	};
}