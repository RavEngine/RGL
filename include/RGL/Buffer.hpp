#pragma once
#include <span>

namespace RGL {

	struct BufferConfig {
		uint32_t size_bytes = 0;
		uint32_t stride = 0;
		enum class Type : uint16_t {
			UniformBuffer = 0x00000010,
			StorageBuffer = 0x00000020,
			IndexBuffer = 0x00000040,
			VertexBuffer = 0x00000080,
			IndirectBuffer = 0x00000100
		} type;
		BufferConfig(decltype(size_bytes) size, decltype(type) type, decltype(stride) stride) : size_bytes(size), type(type), stride(stride) {}

		template<typename T>
		BufferConfig(decltype(type) type, decltype(stride) stride) : size_bytes(sizeof(T)), type(type), stride(stride) {}

		template<typename T>
		BufferConfig(decltype(type) type, decltype(stride) stride, const T& t) : size_bytes(sizeof(T)), type(type), stride(stride) {}
	};

	struct MutableSpan {
		void* data = nullptr;
		size_t size = 0;
	};

	class untyped_span {
		const void* ptr = nullptr;
		const size_t size_bytes = 0;
	public:
		untyped_span(decltype(ptr) ptr, decltype(size_bytes) size_bytes) : ptr(ptr), size_bytes(size_bytes) {}

		template<typename T> 
		untyped_span(const T&& ptr) : ptr(&ptr), size_bytes(sizeof(T)) {}

		constexpr auto data() const { return ptr; }
		constexpr auto size() const { return size_bytes; }
	};

	struct IBuffer {
		/**
		* Map system RAM for updating this buffer. 
		*/
		virtual void MapMemory() = 0;

		/**
		* Unmap system RAM for updating this buffer.
		*/
		virtual void UnmapMemory() = 0;

		/**
		Update the contents of this buffer. If memory is not mapped, it will become mapped. The memory remains mapped. Intended to be used with UniformBuffers or other data that changes frequently.
		@param newData the data to write into the buffer.
		*/
		virtual void UpdateBufferData(untyped_span newData) = 0;

		/**
		Set the contents of this buffer. Intended to be used with VertexBuffers or other data that changes infrequently or never.
		@param newData the data to write into the buffer.
		*/
		virtual void SetBufferData(untyped_span data) = 0;
	};
}