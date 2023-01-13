#pragma once
#include "CommandBuffer.hpp"
#include "RGLD3D12.hpp"
#include <d3d12.h>
#include <memory>

namespace RGL {
	struct CommandQueueD3D12;
	struct TextureD3D12;

	struct CommandBufferD3D12 : public ICommandBuffer {
		ComPtr<ID3D12GraphicsCommandList2> commandList;
		const std::shared_ptr<CommandQueueD3D12> owningQueue;

		TextureD3D12* currentBackbuffer;

		CommandBufferD3D12(decltype(owningQueue));

		// clear the command buffer, to encode new commands
		void Reset() final;

		// signal that new commands are arriving
		void Begin() final;

		// signal that all commands ahve been encoded
		void End() final;

		void BeginRendering(const BeginRenderingConfig&) final;
		void EndRendering() final;

		void BindPipeline(std::shared_ptr<IRenderPipeline>) final;

		void BindBuffer(std::shared_ptr<IBuffer> buffer, uint32_t offset) final;

		void Draw(uint32_t nVertices, uint32_t nInstances = 1, uint32_t startVertex = 0, uint32_t firstInstance = 0) final;

		void SetViewport(const Viewport&) final;
		void SetScissor(const Scissor&) final;

		// submit onto the queue that created this command buffer
		void Commit(const CommitConfig&) final;

		~CommandBufferD3D12() {}
	};
}