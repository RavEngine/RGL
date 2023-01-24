#pragma once
#include "CommandBuffer.hpp"
#import <Metal/Metal.h>

namespace RGL{
struct CommandQueueMTL;
struct TextureMTL;
struct IBuffer;
    struct CommandBufferMTL : public ICommandBuffer{
        id<MTLCommandBuffer> currentCommandBuffer = nullptr;
        id<MTLRenderCommandEncoder> currentCommandEncoder = nullptr;
        TextureMTL* targetFB = nullptr;
        const std::shared_ptr<CommandQueueMTL> owningQueue;
        CommandBufferMTL(decltype(owningQueue));
        
        // ICommandBuffer
        void Reset() final;
        void Begin() final;
        void End() final;
        void BindPipeline(std::shared_ptr<IRenderPipeline>) final;

        void BeginRendering(const BeginRenderingConfig&) final;
        void EndRendering() final;

        void BindBuffer(std::shared_ptr<IBuffer> buffer, uint32_t offset) final;

        void SetVertexBytes(const untyped_span data, uint32_t offset) final;
        void SetFragmentBytes(const untyped_span data, uint32_t offset) final;

        void Draw(uint32_t nVertices, uint32_t nInstances = 1, uint32_t startVertex = 0, uint32_t firstInstance = 0) final;

        void SetViewport(const Viewport&) final;
        void SetScissor(const Scissor&) final;

        void Commit(const CommitConfig&) final;
        
        virtual ~CommandBufferMTL(){}
    };

}
