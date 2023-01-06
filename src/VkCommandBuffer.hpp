#pragma once
#include "CommandBuffer.hpp"
#include <vulkan/vulkan.h>
#include "RGLVk.hpp"

namespace RGL {
	struct DeviceVk;
	struct CommandQueueVk;

	struct CommandBufferVk : public ICommandBuffer {
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;	// does not need to be destroyed
		VkImage swapchainImage = VK_NULL_HANDLE; // not set in constructor, does not need to be destroyed
		
		const std::shared_ptr<CommandQueueVk> owningQueue;

		CommandBufferVk(decltype(owningQueue) owningQueue);

		~CommandBufferVk();

		// ICommandBuffer
		void Reset() final;
		void Begin() final;
		void End() final;
		void BindPipeline(std::shared_ptr<IRenderPipeline>) final;

		void BeginRendering(const BeginRenderingConfig&) final;
		void EndRendering() final;

		void BindBuffer(std::shared_ptr<IBuffer> buffer, uint32_t offset) final;

		void Draw(uint32_t nVertices, uint32_t nInstances = 1, uint32_t startVertex = 0, uint32_t firstInstance = 0) final;

		void SetViewport(const Viewport&) final;
		void SetScissor(const Scissor&) final;

		void Commit(const CommitConfig&) final;
	};
}