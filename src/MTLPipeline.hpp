#pragma once
#include <RGL/Types.hpp>
#include "MTLObjCCompatLayer.hpp"
#include <RGL/Pipeline.hpp>


namespace RGL{
    struct DeviceMTL;
    
    struct PipelineLayoutMTL : public IPipelineLayout{
        
        PipelineLayoutDescriptor settings;
        PipelineLayoutMTL(const decltype(settings)& settings) : settings(settings){}
        
        void SetLayout(const LayoutConfig& config) final;
        
    };

    struct RenderPipelineMTL : public IRenderPipeline{
        APPLE_API_PTR(MTLRenderPassDescriptor) rpd = nullptr;
        OBJC_ID(MTLRenderPipelineState) pipelineState;
        const std::shared_ptr<DeviceMTL> owningDevice;
        
        RenderPipelineMTL(decltype(owningDevice), const RenderPipelineDescriptor&);
    };

}
