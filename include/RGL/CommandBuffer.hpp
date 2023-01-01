#pragma once
#include <memory>
#include <array>
#include "Texture.hpp"
#include "Buffer.hpp"
#include "Synchronization.hpp"

namespace RGL {
	struct IRenderPipeline;

	struct BindBuffersConfig {
		std::shared_ptr<IBuffer> vertexBuffer;
		uint32_t offset = 0;
	};

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

	struct BindPipelineConfig {
		std::array<float, 4> clearColor{ 0,0,0, 1 };
		ITexture* targetFramebuffer = nullptr;

		BindBuffersConfig buffers;
		Viewport viewport;
		Scissor scissor;
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

		virtual void BindPipeline(std::shared_ptr<IRenderPipeline>, const BindPipelineConfig&) = 0;

		// submit onto the queue that created this command buffer
		virtual void Commit(const CommitConfig&) = 0;
	};
}