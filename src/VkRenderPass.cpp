#if RGL_VK_AVAILABLE
#include "VkRenderPass.hpp"
#include "RGLVk.hpp"
#include "VkTexture.hpp"

namespace RGL{

    VkFormat RGLFormat2VK_old(RGL::TextureFormat format) {
        switch (format) {
        case TextureFormat::BGRA8_Unorm:
            return VK_FORMAT_B8G8R8A8_SRGB;
        default:
            FatalError("Cannot convert texture format");
        }
    }

    VkAttachmentLoadOp RGL2LoadOp(LoadAccessOperation op) {
        switch (op) {
            case decltype(op)::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
            case decltype(op)::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
            case decltype(op)::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            case decltype(op)::NotAccessed: return VK_ATTACHMENT_LOAD_OP_NONE_EXT;
        }
    }

    VkAttachmentStoreOp RGL2StoreOp(StoreAccessOperation op) {
        switch (op) {
        case decltype(op)::Store: return VK_ATTACHMENT_STORE_OP_STORE;
        case decltype(op)::None: return VK_ATTACHMENT_STORE_OP_NONE;
        case decltype(op)::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }
    }

    RenderPassVk::RenderPassVk(const RenderPassConfig& config) : config(config), textures{config.attachments.size(), nullptr} {
       
    }

	RenderPassVk::~RenderPassVk()
	{
      
	}

    void RenderPassVk::SetAttachmentTexture(uint32_t index, ITexture* texture)
    {
        textures.at(index) = static_cast<TextureVk*>(texture);
    }

}

#endif