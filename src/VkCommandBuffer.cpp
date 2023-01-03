#if RGL_VK_AVAILABLE
#include "VkCommandBuffer.hpp"
#include "VkCommandQueue.hpp"
#include "VkDevice.hpp"
#include "VkRenderPipeline.hpp"
#include "VkTexture.hpp"
#include "VkBuffer.hpp"

namespace RGL {
	void RGL::CommandBufferVk::Reset()
	{
		VK_CHECK(vkResetCommandBuffer(commandBuffer, 0));
	}
	void CommandBufferVk::Begin()
	{
		VkCommandBufferBeginInfo beginInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = 0,
		.pInheritanceInfo = nullptr,
		};
		VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo))
	}
	void CommandBufferVk::End()
	{
		VK_CHECK(vkEndCommandBuffer(commandBuffer));
	}
	void CommandBufferVk::BindPipeline(std::shared_ptr<IRenderPipeline> generic_pipeline, const BindPipelineConfig& config)
	{
		auto pipeline = std::static_pointer_cast<RenderPipelineVk>(generic_pipeline);
		auto texture = static_cast<TextureVk*>(config.targetFramebuffer);
		auto texSize = texture->GetSize();

		VkClearValue clearColor = { {{config.clearColor[0], config.clearColor[1], config.clearColor[2], config.clearColor[3]}}};

		// Update the renderPass's framebuffer if needed
		pipeline->renderPass->UpdateFramebuffer(texSize.width, texSize.height);

		VkRenderPassAttachmentBeginInfo attachment_begin_info = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO,
			.attachmentCount = 1,	//TODO: support multiple attachments
			.pAttachments = &(texture->vkImageView)
		};

		VkRenderPassBeginInfo renderPassInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = &attachment_begin_info,	// imageless framebuffer
		.renderPass = pipeline->renderPass->renderPass,
		.framebuffer = pipeline->renderPass->passFrameBuffer,	// imageless framebuffer
		.renderArea = {
			.offset = {0, 0},
			.extent = VkExtent2D(texSize.width, texSize.height)
		},
		.clearValueCount = 1,
		.pClearValues = &clearColor,
		};

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// drawing commands
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout->layout, 0, 1, &pipeline->pipelineLayout->descriptorSet, 0, nullptr);

		VkBuffer vertexBuffers[] = { std::static_pointer_cast<BufferVk>(config.buffers.vertexBuffer)->buffer };
		VkDeviceSize offsets[] = { config.buffers.offset };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		auto& viewport = config.viewport;
		VkViewport vp{
			.x = viewport.x,
			.y = viewport.y,
			.width = viewport.width,
			.height = viewport.height, // make Vulkan a Y-up system
			.minDepth = viewport.minDepth,
			.maxDepth = viewport.maxDepth
		};

		VkRect2D scissor{
			.offset = {config.scissor.offset[0], config.scissor.offset[1]},
			.extent = {config.scissor.extent[0],config.scissor.extent[1]},
		};
		vkCmdSetViewport(commandBuffer, 0, 1, &vp);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		vkCmdDraw(commandBuffer, config.numVertices, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffer);
	}
	void CommandBufferVk::Commit(const CommitConfig& config)
	{
		owningQueue->Submit(this, config);
	}
	CommandBufferVk::CommandBufferVk(decltype(owningQueue) owningQueue) : owningQueue(owningQueue)
	{
		auto device = owningQueue->owningDevice->device;
		VkCommandBufferAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = owningQueue->owningDevice->commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
		};
		VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer));
	}
	CommandBufferVk::~CommandBufferVk()
	{
		
	}
}

#endif