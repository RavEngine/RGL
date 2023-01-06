#if RGL_VK_AVAILABLE
#include "VkTexture.hpp"

namespace RGL {
	TextureVk::TextureVk(decltype(vkImageView) imageView, decltype(vkImage) image, const Dimension& size) : vkImageView(imageView), vkImage(image), ITexture(size)
	{
	}
	Dimension TextureVk::GetSize() const
	{
		return this->size;
	}
}

#endif