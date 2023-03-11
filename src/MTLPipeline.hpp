#pragma once
#include <RGL/Types.hpp>
#include "MTLObjCCompatLayer.hpp"
#include <RGL/Pipeline.hpp>


namespace RGL{
    struct DeviceMTL;
    
    struct PipelineLayoutMTL : public IPipelineLayout{
                
        PipelineLayoutDescriptor settings;
        PipelineLayoutMTL(const decltype(settings)& settings) : settings(settings){}        
    };

    struct RenderPipelineMTL : public IRenderPipeline{
        OBJC_ID(MTLRenderPipelineState) pipelineState;
        OBJC_ID(MTLDepthStencilState) depthStencilState = nullptr;
        const std::shared_ptr<DeviceMTL> owningDevice;
        RenderPipelineDescriptor settings;
        
        RenderPipelineMTL(decltype(owningDevice), const RenderPipelineDescriptor&);
    };

}
