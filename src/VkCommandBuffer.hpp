#pragma once
#include <RGL/Types.hpp>
#include <RGL/CommandBuffer.hpp>
#include <vulkan/vulkan.h>
#include "RGLVk.hpp"

namespace RGL {
	struct DeviceVk;
	struct CommandQueueVk;
	struct RenderPipelineVk;

	struct CommandBufferVk : public ICommandBuffer {
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;	// does not need to be destroyed
		VkImage swapchainImage = VK_NULL_HANDLE; // not set in constructor, does not need to be destroyed
		
		const std::shared_ptr<CommandQueueVk> owningQueue;
		std::shared_ptr<RenderPipelineVk> currentRenderPipeline = nullptr;

		CommandBufferVk(decltype(owningQueue) owningQueue);

		~CommandBufferVk();

		// ICommandBuffer
		void Reset() final;
		void Begin() final;
		void End() final;
		void BindPipeline(RGLRenderPipelinePtr) final;

		void BeginRendering(const BeginRenderingConfig&) final;
		void EndRendering() final;

		void BindBuffer(RGLBufferPtr buffer, uint32_t offset) final;

		void setPushConstantData(const RGL::untyped_span& data, const uint32_t& offset, decltype(VK_SHADER_STAGE_VERTEX_BIT) stages);

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

		void Commit(const CommitConfig&) final;
	};
}