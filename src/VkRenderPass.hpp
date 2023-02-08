#pragma once
#include <RGL/Types.hpp>
#include <RGL/RenderPass.hpp>
#include "VkDevice.hpp"

namespace RGL {
	struct TextureVk;

	struct RenderPassVk : public IRenderPass {
		RenderPassConfig config;

		RenderPassVk(const RenderPassConfig&);
		virtual ~RenderPassVk();

		std::vector<TextureVk*> textures;

		void SetAttachmentTexture(uint32_t index, ITexture* texture) final;
	};

}