#if RGL_VK_AVAILABLE
#include "VkTexture.hpp"

namespace RGL {
	Dimension TextureVk::GetSize() const
	{
		return this->size;
	}
}

#endif