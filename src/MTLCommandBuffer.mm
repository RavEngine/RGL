#if RGL_MTL_AVAILABLE
#include "MTLCommandBuffer.hpp"
namespace RGL{
CommandBufferMTL::CommandBufferMTL(decltype(owningQueue) owningQueue) : owningQueue(owningQueue){
    
}

void CommandBufferMTL::Reset(){
    
}

void CommandBufferMTL::Begin(){
    
}

void CommandBufferMTL::End(){
    
}

void CommandBufferMTL::BindPipeline(std::shared_ptr<IRenderPipeline> pipeline){
    
}

void CommandBufferMTL::BeginRendering(const BeginRenderingConfig & config){
    
}

void CommandBufferMTL::EndRendering(){
    
}

void CommandBufferMTL::BindBuffer(std::shared_ptr<IBuffer> buffer, uint32_t offset){
    
}

void CommandBufferMTL::SetVertexBytes(const untyped_span data, uint32_t offset){
    
}

void CommandBufferMTL::SetFragmentBytes(const untyped_span data, uint32_t offset){
    
}

void CommandBufferMTL::Draw(uint32_t nVertices, uint32_t nInstances, uint32_t startVertex, uint32_t firstInstance){
    
}

void CommandBufferMTL::SetViewport(const Viewport & viewport){
    
}

void CommandBufferMTL::SetScissor(const Scissor & scissor){
    
}

void CommandBufferMTL::Commit(const CommitConfig & config){
    
}

}
#endif
