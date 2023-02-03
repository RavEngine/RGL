#if RGL_MTL_AVAILABLE
#include "MTLCommandBuffer.hpp"
#include "MTLCommandQueue.hpp"
#include "MTLPipeline.hpp"
#include "MTLTexture.hpp"
#include "MTLBuffer.hpp"
#include "MTLSampler.hpp"

namespace RGL{
CommandBufferMTL::CommandBufferMTL(decltype(owningQueue) owningQueue) : owningQueue(owningQueue){
    
}

void CommandBufferMTL::Reset(){
    currentCommandEncoder = nullptr;
    currentCommandBuffer = nullptr;
    targetFB = nullptr;
}

void CommandBufferMTL::Begin(){
    currentCommandBuffer = [owningQueue->commandQueue commandBuffer];
}

void CommandBufferMTL::End(){
    
}

void CommandBufferMTL::SetIndexBuffer(RGLBufferPtr buffer) {
    indexBuffer = std::static_pointer_cast<BufferMTL>(buffer);
}

void CommandBufferMTL::BindPipeline(std::shared_ptr<IRenderPipeline> pipelineIn){
    auto pipeline = std::static_pointer_cast<RenderPipelineMTL>(pipelineIn);
    [pipeline->rpd.colorAttachments[0] setTexture:[targetFB->drawable texture]];
    [pipeline->rpd.colorAttachments[0] setClearColor:MTLClearColorMake(clearColor[0], clearColor[1], clearColor[2], clearColor[3])];
    currentCommandEncoder = [currentCommandBuffer renderCommandEncoderWithDescriptor:pipeline->rpd];
    [currentCommandEncoder setRenderPipelineState: pipeline->pipelineState];
}

void CommandBufferMTL::BeginRendering(const BeginRenderingConfig & config){
    targetFB = static_cast<TextureMTL*>(config.targetFramebuffer);
    [currentCommandBuffer presentDrawable:targetFB->drawable];
    clearColor = config.clearColor;
}

void CommandBufferMTL::EndRendering(){
    [currentCommandEncoder endEncoding];
}

void CommandBufferMTL::BindBuffer(RGLBufferPtr buffer, uint32_t offset){
    [currentCommandEncoder setVertexBuffer:std::static_pointer_cast<BufferMTL>(buffer)->buffer offset:offset atIndex:0];    //TODO: don't hardcode to vertex stage
}

void CommandBufferMTL::SetVertexBytes(const untyped_span data, uint32_t offset){
    [currentCommandEncoder setVertexBytes: data.data() length:data.size() atIndex: offset+1];
}

void CommandBufferMTL::SetFragmentBytes(const untyped_span data, uint32_t offset){
    [currentCommandEncoder setFragmentBytes: data.data() length:data.size() atIndex: offset+1];

}

void CommandBufferMTL::Draw(uint32_t nVertices, const DrawInstancedConfig& config){
    [currentCommandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:config.startVertex vertexCount:nVertices instanceCount:config.nInstances baseInstance:config.firstInstance];
}

void CommandBufferMTL::DrawIndexed(uint32_t nIndices, const DrawIndexedInstancedConfig& config){
    assert(indexBuffer != nil); // did you forget to call SetIndexBuffer?
    //TODO: support 16-bit indices
    [currentCommandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle indexCount:nIndices indexType:MTLIndexTypeUInt32 indexBuffer:indexBuffer->buffer indexBufferOffset:config.firstIndex instanceCount:config.nInstances baseVertex:config.startVertex baseInstance:config.firstInstance];
    
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

}
#endif
