#pragma once
#include <RGL/Types.hpp>
#include <RGL/Pipeline.hpp>

namespace RGL{
    struct DeviceWG;
    
    struct PipelineLayoutWG : public IPipelineLayout{
                
        PipelineLayoutDescriptor settings;
        PipelineLayoutWG(const decltype(settings)& settings) : settings(settings){}        
    };

    struct RenderPipelineWG : public IRenderPipeline{
        const std::shared_ptr<DeviceWG> owningDevice;
        RenderPipelineDescriptor settings;
        RenderPipelineWG(decltype(owningDevice), const RenderPipelineDescriptor&);
    };

}
