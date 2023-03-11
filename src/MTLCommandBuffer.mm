#if RGL_MTL_AVAILABLE
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include "MTLCommandBuffer.hpp"
#include "MTLCommandQueue.hpp"
#include "MTLPipeline.hpp"
#include "MTLTexture.hpp"
#include "MTLBuffer.hpp"
#include "MTLSampler.hpp"
#include "MTLRenderPass.hpp"
#include "RGLCommon.hpp"

namespace RGL{

MTLWinding rgl2mtlwinding(RGL::WindingOrder order){
    switch (order){
        case decltype(order)::Clockwise: return MTLWindingClockwise;
        case decltype(order)::Counterclockwise: return MTLWindingCounterClockwise;
    }
}

MTLCullMode rgl2mtlcullmode(RGL::CullMode mode){
    switch (mode){
        case decltype(mode)::None: return MTLCullModeNone;
        case decltype(mode)::Front: return MTLCullModeFront;
        case decltype(mode)::Back: return MTLCullModeBack;
        case decltype(mode)::Both:
        default:
            FatalError("Invalid cullmode");
    }
}

CommandBufferMTL::CommandBufferMTL(decltype(owningQueue) owningQueue) : owningQueue(owningQueue){
    
}

void CommandBufferMTL::Reset(){
    currentCommandEncoder = nullptr;
    currentCommandBuffer = nullptr;
    vertexBuffer = nullptr;
    indexBuffer = nullptr;
}

void CommandBufferMTL::Begin(){
    currentCommandBuffer = [owningQueue->commandQueue commandBuffer];
}

void CommandBufferMTL::End(){
    
}

void CommandBufferMTL::SetIndexBuffer(RGLBufferPtr buffer) {
    indexBuffer = std::static_pointer_cast<BufferMTL>(buffer);
}

void CommandBufferMTL::BindPipeline(RGLRenderPipelinePtr pipelineIn){
    auto pipeline = std::static_pointer_cast<RenderPipelineMTL>(pipelineIn);
    [currentCommandEncoder setRenderPipelineState: pipeline->pipelineState];
    if (pipeline->depthStencilState){
        [currentCommandEncoder setDepthStencilState:pipeline->depthStencilState];
    }
    [currentCommandEncoder setFrontFacingWinding:rgl2mtlwinding(pipeline->settings.rasterizerConfig.windingOrder)];
    [currentCommandEncoder setCullMode:rgl2mtlcullmode(pipeline->settings.rasterizerConfig.cullMode)];
}

void CommandBufferMTL::BeginRendering(RGLRenderPassPtr renderPass){
    auto casted = std::static_pointer_cast<RenderPassMTL>(renderPass);
    currentCommandEncoder = [currentCommandBuffer renderCommandEncoderWithDescriptor:casted->renderPassDescriptor];
}

void CommandBufferMTL::EndRendering(){
    [currentCommandEncoder endEncoding];
}

void CommandBufferMTL::BindBuffer(RGLBufferPtr buffer, uint32_t binding, uint32_t offsetIntoBuffer){
    [currentCommandEncoder setVertexBuffer:std::static_pointer_cast<BufferMTL>(buffer)->buffer offset:offsetIntoBuffer atIndex:binding];    //TODO: don't hardcode to vertex stage
}

void CommandBufferMTL::SetVertexBuffer(RGLBufferPtr buffer, uint32_t offsetIntoBuffer) {
    vertexBuffer = std::static_pointer_cast<BufferMTL>(buffer);
    [currentCommandEncoder setVertexBuffer:vertexBuffer->buffer offset:offsetIntoBuffer * vertexBuffer->stride atIndex:0];
}

void CommandBufferMTL::SetVertexBytes(const untyped_span data, uint32_t offset){
    [currentCommandEncoder setVertexBytes: data.data() length:data.size() atIndex: offset+1];
}

void CommandBufferMTL::SetFragmentBytes(const untyped_span data, uint32_t offset){
    [currentCommandEncoder setFragmentBytes: data.data() length:data.size() atIndex: offset+1];

}

void CommandBufferMTL::Draw(uint32_t nVertices, const DrawInstancedConfig& config){
    [currentCommandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:config.startVertex * vertexBuffer->stride vertexCount:nVertices instanceCount:config.nInstances baseInstance:config.firstInstance];
}

void CommandBufferMTL::DrawIndexed(uint32_t nIndices, const DrawIndexedInstancedConfig& config){
    assert(indexBuffer != nil); // did you forget to call SetIndexBuffer?
    auto indexType = MTLIndexTypeUInt32;
    if (indexBuffer->stride == 2){
        indexType = MTLIndexTypeUInt16;
    }
    
    [currentCommandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle indexCount:nIndices indexType:indexType indexBuffer:indexBuffer->buffer indexBufferOffset:config.firstIndex * indexBuffer->stride instanceCount:config.nInstances baseVertex:config.startVertex * vertexBuffer->stride baseInstance:config.firstInstance];
}

void CommandBufferMTL::SetViewport(const Viewport & viewport){
    MTLViewport vp{
        .originX = viewport.x,
        .originY = viewport.y,
        .width = viewport.width,
        .height = viewport.height,
        .znear = viewport.minDepth,
        .zfar = viewport.maxDepth
    };
    
    [currentCommandEncoder setViewport:vp];
}

void CommandBufferMTL::SetScissor(const Scissor & scissor){
    MTLScissorRect sr{
        .x = static_cast<NSUInteger>(scissor.offset[0]),
        .y = static_cast<NSUInteger>(scissor.offset[1]),
        .width = scissor.extent[0],
        .height = scissor.extent[1]
    };
    
    [currentCommandEncoder setScissorRect:sr];
}

void CommandBufferMTL::Commit(const CommitConfig & config){
    [currentCommandBuffer commit];
    [currentCommandBuffer waitUntilCompleted]; // TODO: delete this
}

void CommandBufferMTL::SetVertexSampler(RGLSamplerPtr sampler, uint32_t index) {
    [currentCommandEncoder setVertexSamplerState:std::static_pointer_cast<SamplerMTL>(sampler)->sampler atIndex:index];
}
void CommandBufferMTL::SetFragmentSampler(RGLSamplerPtr sampler, uint32_t index) {
    [currentCommandEncoder setFragmentSamplerState:std::static_pointer_cast<SamplerMTL>(sampler)->sampler atIndex:index];
}

void CommandBufferMTL::SetVertexTexture(const ITexture* texture, uint32_t index){
    [currentCommandEncoder setVertexTexture:static_cast<const TextureMTL*>(texture)->texture atIndex:index];
}
void CommandBufferMTL::SetFragmentTexture(const ITexture* texture, uint32_t index){
    [currentCommandEncoder setFragmentTexture:static_cast<const TextureMTL*>(texture)->texture atIndex:index];
}
void CommandBufferMTL::SetCombinedTextureSampler(RGLSamplerPtr sampler, const RGL::ITexture *texture, uint32_t index) { 
    [currentCommandEncoder setFragmentTexture:static_cast<const TextureMTL*>(texture)->texture atIndex:index];
    [currentCommandEncoder setFragmentSamplerState:std::static_pointer_cast<SamplerMTL>(sampler)->sampler atIndex:index];

}
}
#endif



