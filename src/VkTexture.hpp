#pragma once
#include "Texture.hpp"
#include <vulkan/vulkan.h>

namespace RGL {

	struct TextureVk : public ITexture {
		const VkImageView vkImageView = VK_NULL_HANDLE;
		const VkImage vkImage = VK_NULL_HANDLE;
		TextureVk(decltype(vkImageView) imageView, decltype(vkImage) image, const Dimension& size);

		Dimension GetSize() const final;
	};

}