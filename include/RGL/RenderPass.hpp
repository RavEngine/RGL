#pragma once
#include "TextureFormat.hpp"
#include <vector>
#include <array>
#include <optional>

namespace RGL{
struct ITexture;
struct RenderPassConfig {
    struct AttachmentDesc {
        // formats, multisample count, load and store ops (see VkAttachmentDescription), and layout (from VkAttachmentReference)
        TextureFormat format;
        MSASampleCount sampleCount = MSASampleCount::C1;
        LoadAccessOperation loadOp = LoadAccessOperation::DontCare;
        StoreAccessOperation storeOp = StoreAccessOperation::DontCare;

        LoadAccessOperation stencilLoadOp = LoadAccessOperation::DontCare;
        StoreAccessOperation stencilStoreOp = StoreAccessOperation::DontCare;

        TextureLayout initialLayout;    // TODO: support (vk)
        TextureLayout finalLayout;        // TODO: support (vk)
        std::array<float, 4> clearColor{ 0,0,0, 1 };
        bool shouldTransition = false;
    };
    std::vector<AttachmentDesc> attachments;
};

struct IRenderPass {
    
    virtual void SetAttachmentTexture(uint32_t index, ITexture* texture) = 0;
};
}
