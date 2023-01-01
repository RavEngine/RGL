#pragma once
#include "Texture.hpp"
#include <vulkan/vulkan.h>

namespace RGL {

	struct TextureVk : public ITexture {
		const VkImageView vkImageView = VK_NULL_HANDLE;
		TextureVk(decltype(vkImageView) image, const Dimension& size);

		Dimension GetSize() const final;
	};

}