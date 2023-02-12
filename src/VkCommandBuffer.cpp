#if RGL_VK_AVAILABLE
#include "VkCommandBuffer.hpp"
#include "VkCommandQueue.hpp"
#include "VkDevice.hpp"
#include "VkRenderPipeline.hpp"
#include "VkTexture.hpp"
#include "VkBuffer.hpp"
#include "VkRenderPass.hpp"
#include <cstring>

namespace RGL {
	VkAttachmentLoadOp RGL2LoadOp(LoadAccessOperation op) {
		switch (op) {
		case decltype(op)::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
		case decltype(op)::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
		case decltype(op)::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		case decltype(op)::NotAccessed: return VK_ATTACHMENT_LOAD_OP_NONE_EXT;
		}
	}

	VkAttachmentStoreOp RGL2StoreOp(StoreAccessOperation op) {
		switch (op) {
		case decltype(op)::Store: return VK_ATTACHMENT_STORE_OP_STORE;
		case decltype(op)::None: return VK_ATTACHMENT_STORE_OP_NONE;
		case decltype(op)::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}
	}

	void encodeResourceTransition(VkCommandBuffer commandBuffer, VkImage image, 
		decltype(VkImageMemoryBarrier::srcAccessMask) srcAccessMask,
		decltype(VkImageMemoryBarrier::dstAccessMask) dstAccessMask,
		decltype(VkImageMemoryBarrier::oldLayout) oldLayout,
		decltype(VkImageMemoryBarrier::newLayout) newLayout,
		decltype(decltype(VkImageMemoryBarrier::subresourceRange)::aspectMask) aspectMask,
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask
	) {
		const VkImageMemoryBarrier image_memory_barrier_begin{
					.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
					.srcAccessMask = srcAccessMask,
					.dstAccessMask = dstAccessMask,
					.oldLayout = oldLayout,
					.newLayout = newLayout,
					.image = image,
					.subresourceRange = {
					  .aspectMask = aspectMask,
					  .baseMipLevel = 0,
					  .levelCount = 1,
					  .baseArrayLayer = 0,
					  .layerCount = 1,
					}
		};

		vkCmdPipelineBarrier(
			commandBuffer,
			srcStageMask,  // srcStageMask
			dstStageMask, // dstStageMask
			0,
			0,
			nullptr,
			0,
			nullptr,
			1, // imageMemoryBarrierCount
			&image_memory_barrier_begin // pImageMemoryBarriers
		);
	}

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
	void CommandBufferVk::BindPipeline(RGLRenderPipelinePtr generic_pipeline)
	{
		auto pipeline = std::static_pointer_cast<RenderPipelineVk>(generic_pipeline);
		

		// drawing commands
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout->layout, 0, 1, &pipeline->pipelineLayout->descriptorSet, 0, nullptr);

		currentRenderPipeline = pipeline;

	}
	void CommandBufferVk::BeginRendering(RGLRenderPassPtr renderPassPtr)
	{
		auto renderPass = std::static_pointer_cast<RenderPassVk>(renderPassPtr);
		currentRenderPass = renderPass;

		stackarray(attachmentInfos, VkRenderingAttachmentInfoKHR, renderPass->config.attachments.size());

		auto makeAttachmentInfo = [](const RenderPassConfig::AttachmentDesc& attachment, VkImageView imageView) -> VkRenderingAttachmentInfoKHR {
			VkClearValue clearColor = { {{attachment.clearColor[0], attachment.clearColor[1], attachment.clearColor[2], attachment.clearColor[3]}} };

			return {
				.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
				.imageView = imageView,
				.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
				.loadOp = RGL2LoadOp(attachment.loadOp),
				.storeOp = RGL2StoreOp(attachment.storeOp),
				.clearValue = clearColor,
			};
		};


		uint32_t i = 0;
		for (const auto& attachment : renderPass->config.attachments) {
			
			attachmentInfos[i] = makeAttachmentInfo(attachment, renderPass->textures[i]->vkImageView);

			// the swapchain image may be in the wrong state (present state vs write state) so it needs to be transitioned
			if (attachment.shouldTransition) {
				encodeResourceTransition(commandBuffer, 
					static_cast<TextureVk*>(renderPass->textures[i])->vkImage, 
					VK_ACCESS_NONE,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VK_IMAGE_ASPECT_COLOR_BIT,
					VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				);
			}

			i++;
		}

		// repeat for depth stencil attachment

		auto texSize = renderPass->textures[0]->GetSize();
		VkRenderingAttachmentInfoKHR* depthAttachmentinfo = nullptr;
		VkRenderingAttachmentInfoKHR depthAttachmentInfoData{};

		if (renderPass->config.depthAttachment.has_value()) {
			auto& da = renderPass->config.depthAttachment.value();
			depthAttachmentInfoData = makeAttachmentInfo(da, renderPass->depthTexture->vkImageView);
			depthAttachmentinfo = &depthAttachmentInfoData;
		}

		VkRenderingAttachmentInfoKHR* stencilAttachmentInfo = nullptr;
		VkRenderingAttachmentInfoKHR stencilAttachmentInfoData{};

		if (renderPass->config.stencilAttachment.has_value()) {
			auto& sa = renderPass->config.stencilAttachment.value();
			stencilAttachmentInfoData = makeAttachmentInfo(sa, renderPass->depthTexture->vkImageView);
			stencilAttachmentInfo = &stencilAttachmentInfoData;
		}


		const VkRenderingInfoKHR render_info{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
			.renderArea = {
				.offset = {0,0},
				.extent = VkExtent2D{.width = texSize.width, .height = texSize.height},
			},
			.layerCount = 1,
			.colorAttachmentCount = static_cast<uint32_t>(renderPass->config.attachments.size()),
			.pColorAttachments = attachmentInfos,
			.pDepthAttachment = depthAttachmentinfo,
			.pStencilAttachment = stencilAttachmentInfo
		};


		vkCmdBeginRendering(commandBuffer, &render_info);
	}
	void CommandBufferVk::EndRendering()
	{
		vkCmdEndRendering(commandBuffer);
		currentRenderPipeline = nullptr;	// reset this to avoid having stale state
		uint32_t i = 0;
		for (const auto& attachment : currentRenderPass->config.attachments) {
			if (attachment.shouldTransition) {
				// the swapchain image is not in the correct format for presentation now
				// so it needs to be transitioned 

				encodeResourceTransition(commandBuffer,
					static_cast<TextureVk*>(currentRenderPass->textures[i])->vkImage,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_NONE,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					VK_IMAGE_ASPECT_COLOR_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
				);
			}
			i++;
		}

	}
	void CommandBufferVk::BindBuffer(RGLBufferPtr buffer, uint32_t bindingOffset, uint32_t offsetIntoBuffer)
	{
		VkBuffer vertexBuffers[] = { std::static_pointer_cast<BufferVk>(buffer)->buffer };
		VkDeviceSize offsets[] = { offsetIntoBuffer };
		vkCmdBindVertexBuffers(commandBuffer, bindingOffset, 1, vertexBuffers, offsets);
	}

	void CommandBufferVk::SetVertexBuffer(RGLBufferPtr buffer, uint32_t offsetIntoBuffer)
	{
		VkBuffer vertexBuffers[] = { std::static_pointer_cast<BufferVk>(buffer)->buffer };
		VkDeviceSize offsets[] = { offsetIntoBuffer };
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
	void CommandBufferVk::SetIndexBuffer(RGLBufferPtr buffer)
	{
		//TODO: support 16 bit index buffer
		vkCmdBindIndexBuffer(commandBuffer, std::static_pointer_cast<BufferVk>(buffer)->buffer, 0, VK_INDEX_TYPE_UINT32);
	}
	void CommandBufferVk::SetVertexSampler(RGLSamplerPtr sampler, uint32_t index)
	{
		
	}
	void CommandBufferVk::SetFragmentSampler(RGLSamplerPtr sampler, uint32_t index)
	{
	}
	void CommandBufferVk::SetVertexTexture(const ITexture* texture, uint32_t index)
	{
	}
	void CommandBufferVk::SetFragmentTexture(const ITexture* texture, uint32_t index)
	{
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