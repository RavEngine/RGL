#pragma once
#include <cstdint>

namespace RGL {
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
		untyped_span(const T& ptr) : ptr(&ptr), size_bytes(sizeof(T)) {}

		constexpr auto data() const { return ptr; }
		constexpr auto size() const { return size_bytes; }
	};
}