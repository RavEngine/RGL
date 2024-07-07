#include "Texture.hpp"
#include "RGL.hpp"
#include "VkTexture.hpp"
#include <stdexcept>

namespace RGL {
	uint32_t TextureView::GetReadonlyBindlessTextureHandle() {
		auto backend = RGL::CurrentAPI();
		switch (backend) {
		case API::Direct3D12:
			return texture.dx.srvIDX;
		case API::Vulkan:
			return static_cast<const TextureVk*>(parent)->globalDescriptorIndex;
		default:
			throw std::runtime_error("Current backend does not support bindless texturing.");
		}
		return 0;
	}
}