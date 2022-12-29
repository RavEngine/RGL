#if RGL_VK_AVAILABLE
#include "VkShaderLibrary.hpp"
#include "RGLCommon.hpp"
#include "RGLVk.hpp"

namespace RGL {
	ShaderLibraryVk::ShaderLibraryVk(decltype(owningDevice))
	{
		FatalError("Default library is not available on Vulkan");
	}
	ShaderLibraryVk::ShaderLibraryVk(decltype(owningDevice) device, const std::span<uint8_t, std::dynamic_extent> code): owningDevice(device)
	{
		VkShaderModuleCreateInfo createInfo{
		   .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		   .codeSize = code.size(),    // in bytes, not multiples of uint32
		   .pCode = reinterpret_cast<const uint32_t*>(code.data())
		};
		VK_CHECK(vkCreateShaderModule(owningDevice->device, &createInfo, nullptr, &shaderModule))
	}
	ShaderLibraryVk::ShaderLibraryVk(decltype(owningDevice), const std::string_view)
	{
		FatalError("Runtime shader compilation is not yet available on Vulkan");
	}
	ShaderLibraryVk::ShaderLibraryVk(decltype(owningDevice), const std::filesystem::path& path)
	{
		FatalError("Library from file is not available on Vulkan");
	}
	ShaderLibraryVk::~ShaderLibraryVk()
	{
		vkDestroyShaderModule(owningDevice->device, shaderModule, nullptr);
	}
}

#endif