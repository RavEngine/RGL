#if RGL_MTL_AVAILABLE
#import <Metal/Metal.h>
#include "MTLPipeline.hpp"
#include "MTLDevice.hpp"
#include "RGLMTL.hpp"
#include "MTLShaderLibrary.hpp"
#include <simd/simd.h>

namespace RGL{

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

MTLCompareFunction rgl2mtlcomparefunction(DepthCompareFunction fn){
    switch(fn){
        case decltype(fn)::Never:           return MTLCompareFunctionNever;
        case decltype(fn)::Less:            return MTLCompareFunctionLess;
        case decltype(fn)::Equal:           return MTLCompareFunctionEqual;
        case decltype(fn)::LessOrEqual:     return MTLCompareFunctionLessEqual;
        case decltype(fn)::Greater:         return MTLCompareFunctionGreater;
        case decltype(fn)::NotEqual:        return MTLCompareFunctionNotEqual;
        case decltype(fn)::GreaterOrEqual:  return MTLCompareFunctionGreaterEqual;
        case decltype(fn)::Always:          return MTLCompareFunctionAlways;
    }
}

void PipelineLayoutMTL::SetLayout(const LayoutConfig& config){
    this->samplerTextures = config.boundTextures;
}

RenderPipelineMTL::RenderPipelineMTL(decltype(owningDevice) owningDevice, const RenderPipelineDescriptor& desc) : owningDevice(owningDevice), settings(desc){
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
    
    pipelineDesc.depthAttachmentPixelFormat = rgl2mtlformat(desc.depthStencilConfig.depthFormat);
    
    MTL_CHECK(pipelineState = [owningDevice->device newRenderPipelineStateWithDescriptor:pipelineDesc error:&err]);
    
    if (desc.depthStencilConfig.depthTestEnabled){
        auto depthDesc = [MTLDepthStencilDescriptor new];
        depthDesc.depthCompareFunction = rgl2mtlcomparefunction(desc.depthStencilConfig.depthFunction);
        depthDesc.depthWriteEnabled = desc.depthStencilConfig.depthWriteEnabled;
        depthStencilState = [owningDevice->device newDepthStencilStateWithDescriptor:depthDesc];
    }
    
}

}
#endif
