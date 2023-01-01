#if RGL_VK_AVAILABLE
#include "VkTexture.hpp"

namespace RGL {
	TextureVk::TextureVk(decltype(vkImageView) image, const Dimension& size) : vkImageView(image), ITexture(size)
	{
	}
	Dimension TextureVk::GetSize() const
	{
		return this->size;
	}
}

#endif