#pragma once
#include <RGL/Span.hpp>

namespace RGL {

	enum class BufferFlags : int {
		None = 0,
		TransferDestination = 1 << 0,
		TransferSource = 1 << 1
	};

	

	inline BufferFlags operator|(BufferFlags x, BufferFlags y) {
		return BufferFlags{ std::underlying_type_t<BufferFlags>(x) | std::underlying_type_t<BufferFlags>(y) };
	}
	inline BufferFlags operator&(BufferFlags x, BufferFlags y) {
		return BufferFlags{ std::underlying_type_t<BufferFlags>(x) & std::underlying_type_t<BufferFlags>(y) };
	}
	inline BufferFlags& operator|=(BufferFlags& x, BufferFlags y)
	{
		x = x | y;
		return x;
	}
	inline BufferFlags& operator&=(BufferFlags& x, BufferFlags y)
	{
		x = x & y;
		return x;
	}


	enum class BufferAccess : uint8_t {
		Private,
		Shared
	};

	struct BufferConfig {
		uint32_t size_bytes = 0;
		uint32_t stride = 0;
		enum class Type : int {
			NoneDoNotUse = 0,
			UniformBuffer = 0x00000010,
			StorageBuffer = 0x00000020,
			IndexBuffer = 0x00000040,
			VertexBuffer = 0x00000080,
			IndirectBuffer = 0x00000100
		} type;

		BufferAccess access;
		BufferFlags options;

		BufferConfig(decltype(size_bytes) size, decltype(type) type, decltype(stride) stride, decltype(access) access, decltype(options) options = decltype(options)::None) : size_bytes(size), type(type), stride(stride), access(access), options(options) {}

		template<typename T>
		BufferConfig(decltype(type) type, decltype(stride) stride, decltype(access) access, decltype(options) options = decltype(options)::None) : BufferConfig(sizeof(T),type,stride, access, options){}

		template<typename T>
		BufferConfig(decltype(type) type, decltype(stride) stride, const T& t, decltype(access) access, decltype(options) options = decltype(options)::None) : BufferConfig(sizeof(T), type,stride,access,options){}
	};

	inline BufferConfig::Type operator|(BufferConfig::Type x, BufferConfig::Type y) {
		return BufferConfig::Type{ std::underlying_type_t<BufferConfig::Type>(x) | std::underlying_type_t<BufferConfig::Type>(y) };
	}
	inline BufferConfig::Type operator&(BufferConfig::Type x, BufferConfig::Type y) {
		return BufferConfig::Type{ std::underlying_type_t<BufferConfig::Type>(x) & std::underlying_type_t<BufferConfig::Type>(y) };
	}
	inline BufferConfig::Type& operator|=(BufferConfig::Type& x, BufferConfig::Type y)
	{
		x = x | y;
		return x;
	}
	inline BufferConfig::Type& operator&=(BufferConfig::Type& x, BufferConfig::Type y)
	{
		x = x & y;
		return x;
	}

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
		virtual void UpdateBufferData(untyped_span newData, decltype(BufferConfig::size_bytes) offset = 0) = 0;

		/**
		Set the contents of this buffer. Intended to be used with VertexBuffers or other data that changes infrequently or never.
		@param newData the data to write into the buffer.
		*/
		virtual void SetBufferData(untyped_span data, decltype(BufferConfig::size_bytes) offset = 0) = 0;
        
        virtual decltype(BufferConfig::size_bytes) getBufferSize() const = 0;

		virtual void* GetMappedDataPtr() = 0;
	};
}
