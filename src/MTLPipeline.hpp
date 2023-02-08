#pragma once
#include <RGL/Types.hpp>
#include "MTLObjCCompatLayer.hpp"
#include <RGL/Pipeline.hpp>


namespace RGL{
    struct DeviceMTL;
    
    struct PipelineLayoutMTL : public IPipelineLayout{
        
        std::vector<LayoutConfig::TextureAndSampler> samplerTextures;
        
        PipelineLayoutDescriptor settings;
        PipelineLayoutMTL(const decltype(settings)& settings) : settings(settings){}
        
        void SetLayout(const LayoutConfig& config) final;
        
    };

    struct RenderPipelineMTL : public IRenderPipeline{
        OBJC_ID(MTLRenderPipelineState) pipelineState;
        const std::shared_ptr<DeviceMTL> owningDevice;
        RenderPipelineDescriptor settings;
        
        RenderPipelineMTL(decltype(owningDevice), const RenderPipelineDescriptor&);
    };

}
