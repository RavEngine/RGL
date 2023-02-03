#pragma once
#include <RGL/Types.hpp>
#include <memory>
#include <array>
#include <RGL/Texture.hpp>
#include <RGL/Buffer.hpp>
#include <RGL/Synchronization.hpp>
#include <span>

struct DrawInstancedConfig{
    uint32_t
    nInstances = 1,
    startVertex = 0,
    firstInstance = 0;
};

struct DrawIndexedInstancedConfig{
    uint32_t
    nInstances = 1,
    firstIndex = 0,
    firstInstance = 0,
    startVertex = 0;
};

namespace RGL {
	struct IRenderPipeline;
    struct ISampler;
    struct ITexture;

	struct Viewport {
		float 
			x = 0, 
			y = 0,
			width = 1, 
			height = 1, 
			minDepth = 0, 
			maxDepth = 1;
	};
	struct Scissor {
		std::array<int32_t, 2> offset{ 0,0 };
		std::array<uint32_t, 2> extent{ 0,0, };
	};

	struct BeginRenderingConfig {
		std::array<float, 4> clearColor{ 0,0,0, 1 };
		ITexture* targetFramebuffer = nullptr;
	};

	struct CommitConfig {
		RGLFencePtr signalFence;
		std::span<RGLSemaphorePtr> waitSemaphores, signalSemaphores;
	};

	struct ICommandBuffer {
		// clear the command buffer, to encode new commands
		virtual void Reset() = 0;

		// signal that new commands are arriving
		virtual void Begin() = 0;

		// signal that all commands ahve been encoded
		virtual void End() = 0;

		virtual void BeginRendering(const BeginRenderingConfig&) = 0;
		virtual void EndRendering() = 0;

		virtual void BindPipeline(RGLRenderPipelinePtr) = 0;

		virtual void BindBuffer(RGLBufferPtr buffer, uint32_t offset) = 0;

        virtual void SetIndexBuffer(RGLBufferPtr buffer) = 0;
        
        virtual void SetVertexSampler(RGLSamplerPtr sampler, uint32_t index) = 0;
        virtual void SetFragmentSampler(RGLSamplerPtr sampler, uint32_t index) = 0;
        
        virtual void SetVertexTexture(const ITexture* texture, uint32_t index) = 0;
        virtual void SetFragmentTexture(const ITexture* texture, uint32_t index) = 0;
        
        virtual void Draw(uint32_t nVertices, const DrawInstancedConfig& = {}) = 0;
        virtual void DrawIndexed(uint32_t nIndices, const DrawIndexedInstancedConfig& = {}) = 0;

		virtual void SetViewport(const Viewport&) = 0;
		virtual void SetScissor(const Scissor&) = 0;

		// submit onto the queue that created this command buffer
		virtual void Commit(const CommitConfig&) = 0;

		virtual void SetVertexBytes(const untyped_span data, uint32_t offset) = 0;
		virtual void SetFragmentBytes(const untyped_span data, uint32_t offset) = 0;

	};
}
