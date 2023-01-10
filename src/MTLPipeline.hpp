#pragma once
#include <Metal/Metal.h>
#include "Pipeline.hpp"


namespace RGL{
    struct DeviceMTL;
    
    struct PipelineLayoutMTL : public IPipelineLayout{
        
        PipelineLayoutDescriptor settings;
        PipelineLayoutMTL(const decltype(settings)& settings) : settings(settings){}
        
        void SetLayout(const LayoutConfig& config) final;
        
    };

    struct RenderPipelineMTL : public IRenderPipeline{
        MTLRenderPassDescriptor* rpd = nullptr;
        id<MTLRenderPipelineState> pipelineState;
        const std::shared_ptr<DeviceMTL> owningDevice;
        
        RenderPipelineMTL(decltype(owningDevice), const RenderPipelineDescriptor&);
    };

}
