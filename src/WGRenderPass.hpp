#pragma once
#include <RGL/RenderPass.hpp>
#include <array>
#include <emscripten/html5_webgpu.h>

namespace RGL{
    struct ITexture;
    
    struct RenderPassWG : public IRenderPass{

        WGPURenderPassDescriptor renderPass;
        std::vector<WGPURenderPassColorAttachment> colorAttachments;
        WGPURenderPassDepthStencilAttachment depthStencilAttachment;

        // IRenderPass
        RenderPassWG(const RenderPassConfig& config);
        virtual ~RenderPassWG(){}
        void SetAttachmentTexture(uint32_t index, ITexture* texture) final;
        
        void SetDepthAttachmentTexture(ITexture* texture) final;
        void SetStencilAttachmentTexture(ITexture* texture) final;
    };
}
