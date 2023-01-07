#if RGL_DX12_AVAILABLE
#include "D3D12ShaderLibrary.hpp"
#include <d3dcompiler.h>

namespace RGL {
	ShaderLibraryD3D12::ShaderLibraryD3D12()
	{
		FatalError("Not implemented");
	}
	ShaderLibraryD3D12::ShaderLibraryD3D12(const std::span<uint8_t, std::dynamic_extent> bytes)
	{
		FatalError("Not implemented");
	}
	ShaderLibraryD3D12::ShaderLibraryD3D12(const std::string_view)
	{
		FatalError("Not implemented");
	}
	ShaderLibraryD3D12::ShaderLibraryD3D12(const std::filesystem::path& path)
	{
		DX_CHECK(D3DReadFileToBlob(path.c_str(), &shaderBlob));
		shaderBytecode = CD3DX12_SHADER_BYTECODE(shaderBlob.Get());
	}
}

#endif