#pragma once
#include "CommandBuffer.hpp"
#import <Metal/Metal.h>
#include <array>

namespace RGL{
struct CommandQueueMTL;
struct TextureMTL;
struct IBuffer;
struct BufferMTL;

    struct CommandBufferMTL : public ICommandBuffer{
        id<MTLCommandBuffer> currentCommandBuffer = nullptr;
        id<MTLRenderCommandEncoder> currentCommandEncoder = nullptr;
        TextureMTL* targetFB = nullptr;
        std::array<float, 4> clearColor{0,0,0,0};
        
        std::shared_ptr<BufferMTL> indexBuffer;
        
        const std::shared_ptr<CommandQueueMTL> owningQueue;
        CommandBufferMTL(decltype(owningQueue));
        
        // ICommandBuffer
        void Reset() final;
        void Begin() final;
        void End() final;
        void BindPipeline(std::shared_ptr<IRenderPipeline>) final;

        void BeginRendering(const BeginRenderingConfig&) final;
        void EndRendering() final;

        void BindBuffer(RGLBufferPtr buffer, uint32_t offset) final;
        
        void SetIndexBuffer(RGLBufferPtr buffer) final;

        void SetVertexBytes(const untyped_span data, uint32_t offset) final;
        void SetFragmentBytes(const untyped_span data, uint32_t offset) final;
        
        void SetVertexSampler(RGLSamplerPtr sampler, uint32_t index) final;
        void SetFragmentSampler(RGLSamplerPtr sampler, uint32_t index) final;
        
        void SetVertexTexture(const ITexture* texture, uint32_t index) final;
        void SetFragmentTexture(const ITexture* texture, uint32_t index) final;

        void Draw(uint32_t nVertices, const DrawInstancedConfig& = {}) final;
        void DrawIndexed(uint32_t nIndices, const DrawIndexedInstancedConfig& = {}) final;

        void SetViewport(const Viewport&) final;
        void SetScissor(const Scissor&) final;

        void Commit(const CommitConfig&) final;
        
        virtual ~CommandBufferMTL(){}
    };

}
