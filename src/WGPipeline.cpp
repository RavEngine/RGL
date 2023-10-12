#if RGL_WEBGPU_AVAILABLE
#include "WGPipeline.hpp"
#include "WGDevice.hpp"

namespace RGL{

RenderPipelineWG::RenderPipelineWG(decltype(owningDevice) owningDevice, const RenderPipelineDescriptor& desc) : owningDevice(owningDevice){
    WGPURenderPipelineDescriptor pipelineDesc{};
    pipelineDesc.nextInChain = nullptr;

    renderPipeline = wgpuDeviceCreateRenderPipeline(owningDevice->device, &pipelineDesc);
}
RenderPipelineWG::~RenderPipelineWG(){
    wgpuRenderPipelineRelease(renderPipeline);
}

}

#endif