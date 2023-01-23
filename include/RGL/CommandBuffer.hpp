#pragma once
#include <memory>
#include <array>
#include "Texture.hpp"
#include "Buffer.hpp"
#include "Synchronization.hpp"

namespace RGL {
	struct IRenderPipeline;


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
		std::shared_ptr<IFence> signalFence;
		std::span<std::shared_ptr<ISemaphore>> waitSemaphores, signalSemaphores;
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

		virtual void BindPipeline(std::shared_ptr<IRenderPipeline>) = 0;

		virtual void BindBuffer(std::shared_ptr<IBuffer> buffer, uint32_t offset) = 0;

		virtual void Draw(uint32_t nVertices, uint32_t nInstances = 1, uint32_t startVertex = 0, uint32_t firstInstance = 0) = 0;

		virtual void SetViewport(const Viewport&) = 0;
		virtual void SetScissor(const Scissor&) = 0;

		// submit onto the queue that created this command buffer
		virtual void Commit(const CommitConfig&) = 0;

		virtual void SetVertexBytes(const untyped_span data, uint32_t offset) = 0;
		virtual void SetFragmentBytes(const untyped_span data, uint32_t offset) = 0;

	};
}