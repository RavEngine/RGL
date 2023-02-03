#pragma once
#include <RGL/Types.hpp>
#include <RGL/CommandBuffer.hpp>
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
		bool ended = false;

		CommandBufferD3D12(decltype(owningQueue));

		// clear the command buffer, to encode new commands
		void Reset() final;

		// signal that new commands are arriving
		void Begin() final;

		// signal that all commands ahve been encoded
		void End() final;

		void BeginRendering(const BeginRenderingConfig&) final;
		void EndRendering() final;

		void BindPipeline(RGLRenderPipelinePtr) final;

		void BindBuffer(RGLBufferPtr buffer, uint32_t offset) final;

		void SetVertexBytes(const untyped_span data, uint32_t offset) final;
		void SetFragmentBytes(const untyped_span data, uint32_t offset) final;

		void SetIndexBuffer(RGLBufferPtr buffer) final;

		void SetVertexSampler(RGLSamplerPtr sampler, uint32_t index) final;
		void SetFragmentSampler(RGLSamplerPtr sampler, uint32_t index) final;

		void SetVertexTexture(const ITexture* texture, uint32_t index) final;
		void SetFragmentTexture(const ITexture* texture, uint32_t index) final;

		void Draw(uint32_t nVertices, const DrawInstancedConfig & = {}) final;
		void DrawIndexed(uint32_t nIndices, const DrawIndexedInstancedConfig & = {}) final;

		void SetViewport(const Viewport&) final;
		void SetScissor(const Scissor&) final;

		// submit onto the queue that created this command buffer
		void Commit(const CommitConfig&) final;

		~CommandBufferD3D12() {}
	};
}