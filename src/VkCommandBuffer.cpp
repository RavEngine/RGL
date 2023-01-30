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
		// the swapchain image is not in the correct format for presentation now
		// so it needs to be transitioned 
		const VkImageMemoryBarrier image_memory_barrier_end{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.image = swapchainImage,
			.subresourceRange = {
			  .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			  .baseMipLevel = 0,
			  .levelCount = 1,
			  .baseArrayLayer = 0,
			  .layerCount = 1,
			}
		};

		vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // srcStageMask
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // dstStageMask
			0,
			0,
			nullptr,
			0,
			nullptr,
			1, // imageMemoryBarrierCount
			&image_memory_barrier_end // pImageMemoryBarriers
		);
		VK_CHECK(vkEndCommandBuffer(commandBuffer));
		swapchainImage = VK_NULL_HANDLE;
	}
	void CommandBufferVk::BindPipeline(std::shared_ptr<IRenderPipeline> generic_pipeline)
	{
		auto pipeline = std::static_pointer_cast<RenderPipelineVk>(generic_pipeline);
		

		// drawing commands
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout->layout, 0, 1, &pipeline->pipelineLayout->descriptorSet, 0, nullptr);

		currentRenderPipeline = pipeline;

	}
	void CommandBufferVk::BeginRendering(const BeginRenderingConfig& config)
	{
		VkClearValue clearColor = { {{config.clearColor[0], config.clearColor[1], config.clearColor[2], config.clearColor[3]}} };
		auto texture = static_cast<TextureVk*>(config.targetFramebuffer);
		auto texSize = texture->GetSize();

		const VkRenderingAttachmentInfoKHR color_attachment_info{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
			.imageView = (texture->vkImageView),
			.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = clearColor,
		};

		const VkRenderingInfoKHR render_info{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
			.renderArea = {
				.offset = {0,0},
				.extent = VkExtent2D(texSize.width, texSize.height),
			},
			.layerCount = 1,
			.colorAttachmentCount = 1,	//TODO: support multiple attachments (make multiple VkRenderingAttachmentInfoKHR structures)
			.pColorAttachments = &color_attachment_info,
		};

		swapchainImage = texture->vkImage;

		// the swapchain image may be in the wrong state (present state vs write state) so it needs to be transitioned
		const VkImageMemoryBarrier image_memory_barrier_begin{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.image = swapchainImage,
			.subresourceRange = {
			  .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			  .baseMipLevel = 0,
			  .levelCount = 1,
			  .baseArrayLayer = 0,
			  .layerCount = 1,
			}
		};

		vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // srcStageMask
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
			0,
			0,
			nullptr,
			0,
			nullptr,
			1, // imageMemoryBarrierCount
			&image_memory_barrier_begin // pImageMemoryBarriers
		);

		vkCmdBeginRendering(commandBuffer, &render_info);
	}
	void CommandBufferVk::EndRendering()
	{
		vkCmdEndRendering(commandBuffer);
		currentRenderPipeline = nullptr;	// reset this to avoid having stale state
	}
	void CommandBufferVk::BindBuffer(std::shared_ptr<IBuffer> buffer, uint32_t offset)
	{
		VkBuffer vertexBuffers[] = { std::static_pointer_cast<BufferVk>(buffer)->buffer };
		VkDeviceSize offsets[] = {offset };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	}

	void CommandBufferVk::setPushConstantData(const RGL::untyped_span& data, const uint32_t& offset, decltype(VK_SHADER_STAGE_VERTEX_BIT) stages)
	{
		// size must be a multiple of 4
		// need to get a little extra space for safety
		auto size = data.size() + (data.size() % 4 != 0 ? 4 : 0);
		stackarray(localdata, std::byte, size);
		std::memset(localdata, 0, size);
		std::memcpy(localdata, data.data(), data.size());

		vkCmdPushConstants(commandBuffer, currentRenderPipeline->pipelineLayout->layout, stages, offset, size, localdata);
	}

	void CommandBufferVk::SetVertexBytes(const untyped_span data, uint32_t offset)
	{
		setPushConstantData(data, offset, VK_SHADER_STAGE_VERTEX_BIT);
	}
	void CommandBufferVk::SetFragmentBytes(const untyped_span data, uint32_t offset)
	{
		setPushConstantData(data, offset, VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	void CommandBufferVk::SetIndexBuffer(std::shared_ptr<IBuffer> buffer)
	{
		//TODO: support 16 bit index buffer
		vkCmdBindIndexBuffer(commandBuffer, std::static_pointer_cast<BufferVk>(buffer)->buffer, 0, VK_INDEX_TYPE_UINT32);
	}
	void CommandBufferVk::Draw(uint32_t nVertices, const DrawInstancedConfig& config)
	{
		vkCmdDraw(commandBuffer, nVertices, config.nInstances, config.startVertex, config.firstInstance);
	}
	void CommandBufferVk::DrawIndexed(uint32_t nIndices, const DrawIndexedInstancedConfig& config)
	{
		vkCmdDrawIndexed(commandBuffer, nIndices, config.nInstances, config.firstIndex, config.startVertex, config.firstInstance);
	}
	void CommandBufferVk::SetViewport(const Viewport& viewport)
	{
		VkViewport vp{
			.x = viewport.x,
			.y = viewport.y,
			.width = viewport.width,
			.height = viewport.height, // make Vulkan a Y-up system
			.minDepth = viewport.minDepth,
			.maxDepth = viewport.maxDepth
		};
		vkCmdSetViewport(commandBuffer, 0, 1, &vp);
	}
	void CommandBufferVk::SetScissor(const Scissor& scissorin)
	{
		VkRect2D scissor{
		.offset = {scissorin.offset[0], scissorin.offset[1]},
		.extent = {scissorin.extent[0], scissorin.extent[1]},
		};
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
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