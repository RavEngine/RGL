#pragma once
#include <RGL/Types.hpp>
#include "TextureFormat.hpp"
#include <vector>
#include <array>
#include <optional>

namespace RGL{
struct ITexture;

enum class ResourceLayout : uint8_t {
    Undefined = 0,
    General = 1,
    ColorAttachmentOptimal = 2,
    DepthStencilAttachmentOptimal = 3,
    DepthStencilReadOnlyOptimal = 4,
    ShaderReadOnlyOptimal = 5,
    TransferSourceOptimal = 6,
    TransferDestinationOptimal,
    Reinitialized,
    DepthReadOnlyStencilAttachmentOptimal,
    DepthAttachmentStencilReadOnlyOptimal,
    DepthAttachmentOptimal,
    DepthReadOnlyOptimal,
    StencilAttachmentOptimal,
    StencilReadOnlyOptimal,
    ReadOnlyOptimal,
    AttachmentOptimal,
    Present
};

struct TransitionInfo {
    ResourceLayout beforeLayout, afterLayout;
};

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

        
        std::optional<TransitionInfo> preTransition;   // not all resources need to transition
        std::optional<TransitionInfo> postTransition;  
    };
    std::vector<AttachmentDesc> attachments;

    std::optional<AttachmentDesc> depthAttachment, stencilAttachment;
};

struct IRenderPass {
    
    virtual void SetAttachmentTexture(uint32_t index, ITexture* texture) = 0;
    virtual void SetDepthAttachmentTexture(ITexture* texture) = 0;
    virtual void SetStencilAttachmentTexture(ITexture* texture) = 0;
};

RGLRenderPassPtr CreateRenderPass(const RenderPassConfig& config);

}
