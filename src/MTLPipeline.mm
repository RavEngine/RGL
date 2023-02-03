#if RGL_MTL_AVAILABLE
#import <Metal/Metal.h>
#include "MTLPipeline.hpp"
#include "MTLDevice.hpp"
#include "RGLMTL.hpp"
#include "MTLShaderLibrary.hpp"
#include <simd/simd.h>

namespace RGL{

MTLPixelFormat rgl2mtlformat(TextureFormat format){
    switch(format){
        case decltype(format)::BGRA8_Unorm:
            return MTLPixelFormatBGRA8Unorm;
        default:
            FatalError("Texture format not supported");
    }
}

MTLLoadAction rgl2mtlload(LoadAccessOperation op){
    switch(op){
        case LoadAccessOperation::Load:
            return MTLLoadActionLoad;
        case LoadAccessOperation::Clear:
            return MTLLoadActionClear;
        case LoadAccessOperation::DontCare:
        case LoadAccessOperation::NotAccessed:
            return MTLLoadActionDontCare;
    }
};

std::pair<MTLVertexFormat,uint32_t>  rgl2mtlvx(RenderPipelineDescriptor::VertexConfig::VertexAttributeDesc::Format format){
    switch(format){
        case decltype(format)::Undefined:
            FatalError("'Undefined' verted format passed");
            break;
        case decltype(format)::R32G32_SignedFloat:
            return std::make_pair(MTLVertexFormatFloat2, sizeof(float)*2);
        case decltype(format)::R32G32B32_SignedFloat:
            return std::make_pair(MTLVertexFormatFloat3, sizeof(float)*3);
    }
}

void PipelineLayoutMTL::SetLayout(const LayoutConfig& config){
    
}

RenderPipelineMTL::RenderPipelineMTL(decltype(owningDevice) owningDevice, const RenderPipelineDescriptor& desc) : owningDevice(owningDevice){
    auto pipelineDesc = [MTLRenderPipelineDescriptor new];
    
    id<MTLFunction> vertFunc = nullptr, fragFunc = nullptr;
    for(const auto stage : desc.stages){
        auto fn = std::static_pointer_cast<ShaderLibraryMTL>(stage.shaderModule)->function;
        switch(stage.type){
            case decltype(stage.type)::Vertex:
                vertFunc = fn;
                break;
            case decltype(stage.type)::Fragment:
                fragFunc = fn;
                break;
            default:
                FatalError("Stage type is not supported");
        }
    }
    
    [pipelineDesc setVertexFunction:vertFunc];
    [pipelineDesc setFragmentFunction:fragFunc];
    
    // create a single interleaved buffer descriptor
    {
        uint32_t i = 0;
        uint32_t totalStride = 0;
        auto vertexDescriptor = [MTLVertexDescriptor new];
        for(const auto& attribute : desc.vertexConfig.attributeDescs){
            auto vertexAttribute = [MTLVertexAttributeDescriptor new];
            auto formatpair = rgl2mtlvx(attribute.format);
            [vertexAttribute setFormat:formatpair.first];
            [vertexAttribute setOffset:attribute.offset];
            [vertexAttribute setBufferIndex: 0];
            totalStride += formatpair.second;
            
            vertexDescriptor.attributes[i] = vertexAttribute;
            i++;
        }
        
        vertexDescriptor.layouts[0].stride = totalStride;
        [pipelineDesc setVertexDescriptor:vertexDescriptor];
    }
            
    
    for(int i = 0; i < desc.colorBlendConfig.attachments.size(); i++){
        auto& attachment = desc.colorBlendConfig.attachments[i];
        [pipelineDesc.colorAttachments[i] setPixelFormat:rgl2mtlformat(attachment.format)];
    }
    
    MTL_CHECK(pipelineState = [owningDevice->device newRenderPipelineStateWithDescriptor:pipelineDesc error:&err]);


    rpd = [MTLRenderPassDescriptor new];
    //TODO: support setting load store ops
    [rpd.colorAttachments[0] setLoadAction:MTLLoadActionClear];
    [rpd.colorAttachments[0] setStoreAction:MTLStoreActionStore];
    
#if TARGET_OS_IPHONE
    [rpd setDefaultRasterSampleCount:0];
#else
    [rpd setDefaultRasterSampleCount:static_cast<int>(desc.multisampleConfig.sampleCount)];
#endif
    
}

}
#endif
