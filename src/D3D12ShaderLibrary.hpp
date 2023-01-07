#pragma once
#include "ShaderLibrary.hpp"
#include <filesystem>
#include <span>
#include <cstdint>
#include <d3d12.h>
#include <directx/d3dx12.h>
#include "RGLD3D12.hpp"
namespace RGL {

	struct ShaderLibraryD3D12 : public IShaderLibrary {
		ComPtr<ID3DBlob> shaderBlob;
		CD3DX12_SHADER_BYTECODE shaderBytecode;

		ShaderLibraryD3D12();
		ShaderLibraryD3D12(const std::span<uint8_t, std::dynamic_extent>);
		ShaderLibraryD3D12(const std::string_view);
		ShaderLibraryD3D12(const std::filesystem::path& path);
	};
}