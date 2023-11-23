#if RGL_VK_AVAILABLE
#include "VkCommandBuffer.hpp"
#include "VkCommandQueue.hpp"
#include "VkDevice.hpp"
#include "VkRenderPipeline.hpp"
#include "VkTexture.hpp"
#include "VkBuffer.hpp"
#include "VkRenderPass.hpp"
#include "VkSampler.hpp"
#include "VkSwapchain.hpp"
#include "VkComputePipeline.hpp"
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
		
	}

	void RGL::CommandBufferVk::Reset()
	{
		VK_CHECK(vkResetCommandBuffer(commandBuffer, 0));
		vkResetFences(owningQueue->owningDevice->device, 1, &internalFence);
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
		// ensure that all commands have been encoded
		if (renderCommands.size() > 0) {
			EndContext();
		}

		for (const auto swapRsc : swapchainImages) {
			RecordTextureBinding(swapRsc, { VK_IMAGE_LAYOUT_PRESENT_SRC_KHR , true});
		}

		// place resources back in their native state
		for (const auto [texture, written] : activeTextures) {
			if (!swapchainImages.contains(texture)) {
				RecordTextureBinding(texture, {texture->nativeFormat , true});
			}
		}
		swapchainImages.clear();
		activeTextures.clear();

		VK_CHECK(vkEndCommandBuffer(commandBuffer));
		activeBuffers.clear();
	}
	void CommandBufferVk::BindRenderPipeline(RGLRenderPipelinePtr generic_pipeline)
	{
		currentRenderPipeline = std::static_pointer_cast<RenderPipelineVk>(generic_pipeline);
		EncodeCommand(CmdBindRenderPipeline{ generic_pipeline });
	}
	void CommandBufferVk::BeginRendering(RGLRenderPassPtr renderPassPtr)
	{
		EncodeCommand(CmdBeginRendering{ renderPassPtr });

		auto renderPass = std::static_pointer_cast<RenderPassVk>(renderPassPtr);
		uint32_t i = 0;
		for (const auto& attachment : renderPass->config.attachments) {
			RecordTextureBinding(static_cast<const TextureVk*>(renderPass->textures[i].parent), {
					.lastLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					.written = true
				}
			);
			i++;
		}

		if (renderPass->config.depthAttachment.has_value()) {
			RecordTextureBinding(static_cast<const TextureVk*>(renderPass->depthTexture->parent),
				{
					.lastLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
					.written = true
				}
			);
		}

		if (renderPass->config.stencilAttachment.has_value()) {
			RecordTextureBinding(static_cast<const TextureVk*>(renderPass->stencilTexture->parent),
				{
					.lastLayout = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
					.written = true
				}
			);
		}
	}

	void CommandBufferVk::EndRendering()
	{
		EndContext();
		vkCmdEndRendering(commandBuffer);
		currentRenderPipeline = nullptr;	// reset this to avoid having stale state
	}
	void CommandBufferVk::BeginCompute(RGLComputePipelinePtr inPipeline)
	{
		currentComputePipeline = std::static_pointer_cast<ComputePipelineVk>(inPipeline);
		EncodeCommand(CmdBeginCompute{ inPipeline });
	}
	void CommandBufferVk::EndCompute()
	{
		EncodeCommand(CmdEndCompute{});
		EndContext();
		currentComputePipeline = nullptr;
	}
	void CommandBufferVk::DispatchCompute(uint32_t threadsX, uint32_t threadsY, uint32_t threadsZ,  uint32_t threadsPerThreadgroupX, uint32_t threadsPerThreadgroupY, uint32_t threadsPerThreadgroupZ)
	{
		EncodeCommand(CmdDispatch{ threadsX, threadsY, threadsZ });
	}
	void CommandBufferVk::BindBuffer(RGLBufferPtr buffer, uint32_t bindingOffset, uint32_t offsetIntoBuffer)
	{
		GenericBindBuffer(buffer, offsetIntoBuffer, bindingOffset, VK_PIPELINE_BIND_POINT_GRAPHICS);
	}

	void CommandBufferVk::GenericBindBuffer(RGLBufferPtr& buffer, const uint32_t& offsetIntoBuffer, const uint32_t& bindingOffset, VkPipelineBindPoint bindPoint)
	{
		RecordBufferBinding(std::static_pointer_cast<BufferVk>(buffer).get(), { .written = IsBufferSlotWritable(bindingOffset)});
		EncodeCommand(CmdBindBuffer{buffer, offsetIntoBuffer, bindingOffset, bindPoint});
	}

	void CommandBufferVk::BindComputeBuffer(RGLBufferPtr buffer, uint32_t binding, uint32_t offsetIntoBuffer)
	{
		GenericBindBuffer(buffer, offsetIntoBuffer, binding, VK_PIPELINE_BIND_POINT_COMPUTE);
	}

	void CommandBufferVk::SetVertexBuffer(RGLBufferPtr buffer, const VertexBufferBinding& bindingInfo)
	{
		RecordBufferBinding(std::static_pointer_cast<BufferVk>(buffer).get(), { .written = false });
		EncodeCommand(CmdSetVertexBuffer{ buffer,bindingInfo });
	}

	void CommandBufferVk::setPushConstantData(const RGL::untyped_span& data, const uint32_t& offset)
	{
		Assert(data.size() <= 128, "Push constant data size must be no more than 128 bytes");
		CmdSetPushConstantData cmd{
			// size must be a multiple of 4
			// need to get a little extra space for safety
			.size = uint32_t((data.size() + (data.size() % 4 != 0 ? 4 : 0))),
			.offset = offset,
		};
		std::memcpy(cmd.data, data.data(), data.size());
		EncodeCommand(cmd);
	}

	void CommandBufferVk::SetVertexBytes(const untyped_span data, uint32_t offset)
	{
		setPushConstantData(data, offset);
	}
	void CommandBufferVk::SetFragmentBytes(const untyped_span data, uint32_t offset)
	{
		setPushConstantData(data, offset);
	}
	void CommandBufferVk::SetComputeBytes(const untyped_span data, uint32_t offset)
	{
		setPushConstantData(data, offset);
	}
	void CommandBufferVk::SetIndexBuffer(RGLBufferPtr buffer)
	{
		RecordBufferBinding(std::static_pointer_cast<BufferVk>(buffer).get(), { .written = false });
		EncodeCommand( CmdSetIndexBuffer{buffer} );
	}
	void CommandBufferVk::SetVertexSampler(RGLSamplerPtr sampler, uint32_t index)
	{
		EncodeCommand(CmdSetSampler{ sampler, index });
	}
	void CommandBufferVk::SetFragmentSampler(RGLSamplerPtr sampler, uint32_t index)
	{
		EncodeCommand(CmdSetSampler{ sampler, index });
	}
	void CommandBufferVk::SetComputeSampler(RGLSamplerPtr sampler, uint32_t index)
	{
		EncodeCommand(CmdSetSampler{ sampler, index, true });
	}
	void CommandBufferVk::SetVertexTexture(const TextureView& texture, uint32_t index)
	{
		SetFragmentTexture(texture, index);
	}
	void CommandBufferVk::SetFragmentTexture(const TextureView& texture, uint32_t index)
	{
		EncodeCommand(CmdSetTexture{ texture, index });
		
		auto vktexture = static_cast<const TextureVk*>(texture.parent);


		auto nextLayout = vktexture->createdConfig.usage.DepthStencilAttachment ? VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		auto activeLayout = currentRenderPipeline ? currentRenderPipeline->pipelineLayout : currentComputePipeline->pipelineLayout;
		if (activeLayout->bindingDescriptorTypes.at(index) == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
			nextLayout = VK_IMAGE_LAYOUT_GENERAL;
		}

		RecordTextureBinding(vktexture, {
			.lastLayout = nextLayout,
			.written = true
		});
	}
	void CommandBufferVk::SetComputeTexture(const TextureView& texture, uint32_t index)
	{
		SetFragmentTexture(texture, index);
	}
	void CommandBufferVk::Draw(uint32_t nVertices, const DrawInstancedConfig& config)
	{
		EncodeCommand(CmdDraw{ nVertices, config });
	}
	void CommandBufferVk::DrawIndexed(uint32_t nIndices, const DrawIndexedInstancedConfig& config)
	{
		EncodeCommand(CmdDrawIndexed{nIndices, config});
	}

	void CommandBufferVk::CopyTextureToBuffer(TextureView& sourceTexture, const Rect& sourceRect, size_t offset, RGLBufferPtr destBuffer)
	{
		auto castedDest = std::static_pointer_cast<BufferVk>(destBuffer);

		EncodeCommand(CmdCopyTextureToBuffer{
			sourceTexture,
			sourceRect,
			offset,
			destBuffer
		});

		RecordTextureBinding(static_cast<const TextureVk*>(sourceTexture.parent), { VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, true });
	}
	void CommandBufferVk::CopyBufferToBuffer(BufferCopyConfig from, BufferCopyConfig to, uint32_t size)
	{
		RecordBufferBinding(std::static_pointer_cast<BufferVk>(from.buffer).get(), {.written = false});
		RecordBufferBinding(std::static_pointer_cast<BufferVk>(to.buffer).get(), {.written = true});
		EncodeCommand(CmdCopyBufferToBuffer{ from,to,size });
	}
	void CommandBufferVk::CopyTextureToTexture(const TextureCopyConfig& from, const TextureCopyConfig& to)
	{
		RecordTextureBinding(static_cast<const TextureVk*>(from.texture.parent), {}, true);
		RecordTextureBinding(static_cast<const TextureVk*>(to.texture.parent), {}, true);
		EncodeCommand(CmdCopyTextureToTexture{from,to});
	}
	void CommandBufferVk::SetViewport(const Viewport& viewport)
	{
		EncodeCommand(CmdSetViewport{ viewport });
	}
	void CommandBufferVk::SetScissor(const Rect& scissorin)
	{
		EncodeCommand(CmdSetScissor{ scissorin });
	}
	void CommandBufferVk::Commit(const CommitConfig& config)
	{
		owningQueue->Submit(this, config, internalFence);
		vmaSetCurrentFrameIndex(owningQueue->owningDevice->vkallocator, owningQueue->owningDevice->frameIndex++);
		swapchainsToSignal.clear();
	}

	void CommandBufferVk::ExecuteIndirect(const IndirectConfig& config)
	{
		RecordBufferBinding(std::static_pointer_cast<BufferVk>(config.indirectBuffer).get(), { .written = false });
		EncodeCommand(CmdExecuteIndirect{ config });
	}
	void CommandBufferVk::BeginRenderDebugMarker(const std::string& label)
	{
#ifndef NDEBUG
		EncodeCommand(CmdBeginDebugMarker{ label });
#endif
	}
	void CommandBufferVk::BeginComputeDebugMarker(const std::string& label)
	{
		BeginRenderDebugMarker(label);
	}
	void CommandBufferVk::EndRenderDebugMarker()
	{
#ifndef NDEBUG
		EncodeCommand( CmdEndDebugMarker{} );
#endif
	}
	void CommandBufferVk::EndComputeDebugMarker()
	{
		EndRenderDebugMarker();
	}
	void CommandBufferVk::BlockUntilCompleted()
	{
		vkWaitForFences(owningQueue->owningDevice->device, 1, &internalFence, VK_TRUE, UINT64_MAX);
	}
	void CommandBufferVk::ExecuteIndirectIndexed(const IndirectConfig& config)
	{
		RecordBufferBinding(std::static_pointer_cast<BufferVk>(config.indirectBuffer).get(), { .written = false });
		EncodeCommand(CmdExecuteIndirectIndexed{ config });
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

		VkFenceCreateInfo fenceInfo{
		  .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		  .flags = VkFenceCreateFlags(0)
		};
		VK_CHECK(vkCreateFence(owningQueue->owningDevice->device, &fenceInfo, nullptr, &internalFence));
	}
	CommandBufferVk::~CommandBufferVk()
	{
		vkDestroyFence(owningQueue->owningDevice->device, internalFence, nullptr);
	}

	void CommandBufferVk::RecordBufferBinding(const BufferVk* buffer, BufferLastUse usage)
	{
		auto recUsage = [&]() {
			activeBuffers[buffer] = usage;
		};

		auto it = activeBuffers.find(buffer);

		if (it == activeBuffers.end()) {
			// record the usage but don't set a barrier
			recUsage();
			return;
		}

		if (!it->second.written) {
			recUsage();
			return;
		}
		
		// a previous usage might have changed the data, so we need to sync
		auto owningDeviceFamily = buffer->owningDevice->indices.graphicsFamily.value();
		VkBufferMemoryBarrier2 bufferBarrier{
			.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
				.pNext = nullptr,
				.srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT | VK_ACCESS_2_HOST_WRITE_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				.dstAccessMask = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT | VK_ACCESS_2_INDEX_READ_BIT | VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_2_UNIFORM_READ_BIT | VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_TRANSFER_READ_BIT | VK_ACCESS_2_HOST_READ_BIT | VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
				.srcQueueFamilyIndex = owningDeviceFamily,
				.dstQueueFamilyIndex = owningDeviceFamily,
				.buffer = buffer->buffer,
				.offset = 0,
				.size = buffer->getBufferSize()
		};
		barriersToAdd.push_back(bufferBarrier);

		
		recUsage();
	}

	void CommandBufferVk::RecordTextureBinding(const TextureVk* texture, TextureLastUse usage, bool recordOnly)
	{
		auto it = activeTextures.find(texture);
		auto needed = usage.lastLayout;

		if (it == activeTextures.end()) {
			activeTextures[texture] = {
				.lastLayout = texture->nativeFormat,
				.written = false
			};
			it = activeTextures.find(texture);
		}

		auto current = (*it).second.lastLayout;
		if (current == needed) {
			return;
		}

		if (recordOnly) {
			return;
		}

		VkImageMemoryBarrier transitionbarrier{
					.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
					.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT,
					.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
					.oldLayout = current,
					.newLayout = needed,
					.image = texture->vkImage,
					.subresourceRange = {
					  .aspectMask = texture->createdAspectVk,
					  .baseMipLevel = 0,
					  .levelCount = VK_REMAINING_MIP_LEVELS,
					  .baseArrayLayer = 0,
					  .layerCount = VK_REMAINING_ARRAY_LAYERS,
					} 
		};

		// insert the transition barrier
		vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,  // srcStageMask	
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, // dstStageMask
			0,
			0,
			nullptr,
			0,
			nullptr,
			1, // imageMemoryBarrierCount
			&transitionbarrier // pImageMemoryBarriers
		);

		// update tracker
		(*it).second = {
			.lastLayout = needed,
			.written = usage.written
		};
	}

	template<typename ... Ts>
	struct Overload : Ts ... {
		using Ts::operator() ...;
	};
	template<class... Ts> Overload(Ts...) -> Overload<Ts...>;
	void CommandBufferVk::EndContext()
	{
		auto visitor = Overload{
			[this](const CmdBeginRendering& arg) mutable {
				ApplyBarriers();

				auto renderPass = std::static_pointer_cast<RenderPassVk>(arg.pass);

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

					attachmentInfos[i] = makeAttachmentInfo(attachment, renderPass->textures[i].texture.vk);

					// the swapchain image may be in the wrong state (present state vs write state) so it needs to be transitioned
					auto castedImage = static_cast<const TextureVk*>(renderPass->textures[i].parent);

					if (castedImage->owningSwapchain) {
						swapchainsToSignal.insert(castedImage->owningSwapchain);
						swapchainImages.insert(castedImage);
					}

					i++;
				}

				// repeat for depth stencil attachment
				RGL::Dimension texSize{};
				if (renderPass->textures.size() > 0) {
					texSize = renderPass->textures[0].viewSize;
				}
				else if (renderPass->depthTexture) {
					texSize = renderPass->depthTexture->viewSize;
				}
				else {
					FatalError("No rendertargets are bound, cannot get texture size for beginRendering");
				}

				VkRenderingAttachmentInfoKHR* depthAttachmentinfo = nullptr;
				VkRenderingAttachmentInfoKHR depthAttachmentInfoData{};

				if (renderPass->config.depthAttachment.has_value()) {
					auto& da = renderPass->config.depthAttachment.value();
					depthAttachmentInfoData = makeAttachmentInfo(da, renderPass->depthTexture->texture.vk);
					depthAttachmentinfo = &depthAttachmentInfoData;
				}

				VkRenderingAttachmentInfoKHR* stencilAttachmentInfo = nullptr;
				VkRenderingAttachmentInfoKHR stencilAttachmentInfoData{};

				if (renderPass->config.stencilAttachment.has_value()) {
					auto& sa = renderPass->config.stencilAttachment.value();
					stencilAttachmentInfoData = makeAttachmentInfo(sa, renderPass->depthTexture->texture.vk);
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
			},
			[this](const CmdSetVertexBuffer& arg) {
				auto vkbuffer = std::static_pointer_cast<BufferVk>(arg.buffer);
				VkBuffer vertexBuffers[] = { vkbuffer->buffer };
				VkDeviceSize offsets[] = { arg.bindingInfo.offsetIntoBuffer * vkbuffer->stride };
				vkCmdBindVertexBuffers(commandBuffer, arg.bindingInfo.bindingPosition, std::size(vertexBuffers), vertexBuffers, offsets);
			},
			[this](const CmdSetIndexBuffer& arg) {
				const auto casted = std::static_pointer_cast<BufferVk>(arg.buffer);
				const auto size_type = casted->stride == sizeof(uint16_t) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
				vkCmdBindIndexBuffer(commandBuffer, casted->buffer, 0, size_type);
			},
			[this](const CmdSetSampler& arg) {
				auto& sampler = arg.sampler;
				auto index = arg.index;
				VkDescriptorImageInfo imginfo{
							.sampler = std::static_pointer_cast<SamplerVk>(sampler)->sampler,
							.imageView = VK_NULL_HANDLE,
							.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				};
				VkWriteDescriptorSet writeinfo{
						.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
						.dstSet = VK_NULL_HANDLE,
						.dstBinding = index,
						.dstArrayElement = 0,
						.descriptorCount = 1,
						.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
						.pImageInfo = &imginfo,
						.pBufferInfo = nullptr,
						.pTexelBufferView = nullptr
				};
				owningQueue->owningDevice->vkCmdPushDescriptorSetKHR(
					commandBuffer,
					arg.isCompute ? VK_PIPELINE_BIND_POINT_COMPUTE :  VK_PIPELINE_BIND_POINT_GRAPHICS,
					arg.isCompute ? currentComputePipeline->pipelineLayout->layout : currentRenderPipeline->pipelineLayout->layout,
					0,
					1,
					&writeinfo
				);
			},
			[this](const CmdSetTexture& arg) {
				auto texture = arg.texture;
				auto index = arg.index;
				auto castedImage = static_cast<const TextureVk*>(texture.parent);

				auto it = activeTextures.find(castedImage);
				auto layout = castedImage->nativeFormat;
				if (it != activeTextures.end()) {
					layout = it->second.lastLayout;
				}
				bool isCompute = currentRenderPipeline ? false : true;
				auto activeLayout = isCompute ? currentComputePipeline->pipelineLayout : currentRenderPipeline->pipelineLayout;

				VkDescriptorImageInfo imginfo{
							.sampler = VK_NULL_HANDLE,
							.imageView = texture.texture.vk,
							.imageLayout = layout,
				};
				VkWriteDescriptorSet writeinfo{
						.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
						.dstSet = VK_NULL_HANDLE,
						.dstBinding = index,
						.dstArrayElement = 0,
						.descriptorCount = 1,
						.descriptorType = activeLayout->bindingDescriptorTypes.at(index),
						.pImageInfo = &imginfo,
						.pBufferInfo = nullptr,
						.pTexelBufferView = nullptr
				};
				

				owningQueue->owningDevice->vkCmdPushDescriptorSetKHR(
					commandBuffer,
					isCompute ? VK_PIPELINE_BIND_POINT_COMPUTE : VK_PIPELINE_BIND_POINT_GRAPHICS,
					activeLayout->layout,
					0,
					1,
					&writeinfo
				);

				if (castedImage->owningSwapchain) {
					swapchainsToSignal.insert(castedImage->owningSwapchain);
					swapchainImages.insert(castedImage);
				}
			},
			[this](const CmdDraw& arg) {
				vkCmdDraw(commandBuffer, arg.nVertices, arg.config.nInstances, arg.config.startVertex, arg.config.firstInstance);
			},
			[this](const CmdDrawIndexed& arg) {
				vkCmdDrawIndexed(commandBuffer, arg.nIndices, arg.config.nInstances, arg.config.firstIndex, arg.config.startVertex, arg.config.firstInstance);
			},
			[this](const CmdBindBuffer& arg) {
				auto buffer = arg.buffer;
				auto offsetIntoBuffer = arg.offsetIntoBuffer;
				auto bindingOffset = arg.bindingOffset;
				auto bindPoint = arg.bindPoint;
				auto vkbuffer = std::static_pointer_cast<BufferVk>(buffer);
				VkDescriptorBufferInfo bufferInfo{
					.buffer = vkbuffer->buffer,
					.offset = offsetIntoBuffer * vkbuffer->stride,
					.range = VK_WHOLE_SIZE,
				};
				VkWriteDescriptorSet writeinfo{
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.dstSet = VK_NULL_HANDLE,	// we push descriptors instead
					.dstBinding = bindingOffset,
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					.pImageInfo = nullptr,
					.pBufferInfo = &bufferInfo,
					.pTexelBufferView = nullptr
				};
				owningQueue->owningDevice->vkCmdPushDescriptorSetKHR(
					commandBuffer,
					bindPoint,
					(bindPoint == VK_PIPELINE_BIND_POINT_COMPUTE ? currentComputePipeline->pipelineLayout->layout : currentRenderPipeline->pipelineLayout->layout),
					0,
					1,
					&writeinfo
				);
			},
			[this](const CmdExecuteIndirectIndexed& arg) {
				auto& config = arg.config;
				const auto buffer = std::static_pointer_cast<BufferVk>(config.indirectBuffer);
				vkCmdDrawIndexedIndirect(commandBuffer,
					buffer->buffer,
					config.offsetIntoBuffer,
					config.nDraws,
					sizeof(IndirectIndexedCommand)
				);
			},
			[this](const CmdExecuteIndirect& arg) {
				auto& config = arg.config;
				const auto buffer = std::static_pointer_cast<BufferVk>(config.indirectBuffer);
				vkCmdDrawIndirect(commandBuffer,
					buffer->buffer,
					config.offsetIntoBuffer,
					config.nDraws,
					sizeof(IndirectCommand)
				);
			},
			[this](const CmdSetPushConstantData& arg) {
				auto data = arg.data;

				if (currentRenderPipeline) {
					const auto& flagsStore = currentRenderPipeline->pipelineLayout->pushConstantBindingStageFlags;

					auto stages = flagsStore.contains(arg.offset) ? flagsStore.at(arg.offset) : VK_SHADER_STAGE_COMPUTE_BIT;

					auto layout = currentRenderPipeline->pipelineLayout->layout;

					vkCmdPushConstants(commandBuffer, layout, stages, arg.offset, arg.size, data);
				}
				else {
					const auto& flagsStore = currentComputePipeline->pipelineLayout->pushConstantBindingStageFlags;

					auto stages = VK_SHADER_STAGE_COMPUTE_BIT;

					auto layout = currentComputePipeline->pipelineLayout->layout;

					vkCmdPushConstants(commandBuffer, layout, stages, arg.offset, arg.size, data);

					assert(currentComputePipeline != nullptr);
				}

				
			},
			[this](const CmdBindRenderPipeline& arg) {
				auto pipeline = std::static_pointer_cast<RenderPipelineVk>(arg.generic_pipeline);
				// drawing commands
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);
				currentRenderPipeline = pipeline;
			},
			[this](const CmdBeginDebugMarker& arg) {
				VkDebugUtilsLabelEXT markerInfo = {
				.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
				.pLabelName = arg.label.c_str()
					};
				owningQueue->owningDevice->rgl_vkCmdBeginDebugUtilsLabelEXT(commandBuffer, &markerInfo);
			},
			[this](const CmdEndDebugMarker& ) {
				owningQueue->owningDevice->rgl_vkCmdEndDebugUtilsLabelEXT(commandBuffer);
			},
			[this](const CmdBeginCompute& arg) {
				ApplyBarriers();
				currentComputePipeline = std::static_pointer_cast<ComputePipelineVk>(arg.inPipeline);
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, currentComputePipeline->computePipeline);
			},
			[this](const CmdEndCompute&) {
				currentComputePipeline = nullptr;
			},
			[this](const CmdDispatch& arg) {
				vkCmdDispatch(commandBuffer, arg.threadsX, arg.threadsY, arg.threadsZ);
			},
			[this](const CmdCopyTextureToBuffer& arg) {
				auto castedDest = std::static_pointer_cast<BufferVk>(arg.destBuffer);
				auto castedImage = static_cast<const TextureVk*>(arg.sourceTexture.parent);

				VkBufferImageCopy region{
					.bufferOffset = 0,
					.bufferRowLength = 0,
					.bufferImageHeight = 0,
					.imageSubresource = {
						.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
						.mipLevel = 0,
						.baseArrayLayer = 0,
						.layerCount = 1
					},
					.imageOffset = {
						.x = arg.sourceRect.offset[0],
						.y = arg.sourceRect.offset[1],
						.z = 0
					},
					.imageExtent = {
						.width = arg.sourceRect.extent[0],
						.height = arg.sourceRect.extent[1],
						.depth = 1
					}
				};

				vkCmdCopyImageToBuffer(commandBuffer, castedImage->vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, castedDest->buffer, 1, &region);
			},
			[this](const CmdCopyTextureToTexture& arg) {
				auto src = static_cast<const TextureVk*>(arg.from.texture.parent);
				auto dst = static_cast<const TextureVk*>(arg.to.texture.parent);

				auto& srcLayout = activeTextures.at(src);
				auto& dstLayout = activeTextures.at(dst);

				auto dim = src->GetSize();
				VkImageCopy2 region{
					.sType = VK_STRUCTURE_TYPE_IMAGE_COPY_2,
					.pNext = nullptr,
					.srcSubresource = {
						.aspectMask = src->createdAspectVk,
						.mipLevel = 0,
						.baseArrayLayer = 0,
						.layerCount = 1
					},
					.srcOffset = {0,0,0},
					.dstSubresource = {
						.aspectMask = dst->createdAspectVk,
						.baseArrayLayer = 0,
						.layerCount = 1
					},
					.dstOffset = {0,0,0},
					.extent = {dim.width, dim.height ,1}
				};
				VkCopyImageInfo2 copyInfo{
					.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2,
					.pNext = nullptr,
					.srcImage = src->vkImage,
					.srcImageLayout = srcLayout.lastLayout,
					.dstImage = dst->vkImage,
					.dstImageLayout = dstLayout.lastLayout,
					.regionCount = 1,
					.pRegions = &region,
				};
				vkCmdCopyImage2(commandBuffer, &copyInfo);
			},
			[this](const CmdSetViewport& arg) {
				auto& viewport = arg.viewport;
				VkViewport vp{
					.x = viewport.x,
					.y = viewport.height - viewport.y,
					.width = viewport.width,
					.height = -viewport.height, // make Vulkan a Y-up system
					.minDepth = viewport.minDepth,
					.maxDepth = viewport.maxDepth
				};
				vkCmdSetViewport(commandBuffer, 0, 1, &vp);
			},
			[this](const CmdSetScissor& arg) {
				auto& scissorin = arg.scissor;
				VkRect2D scissor{
				.offset = {scissorin.offset[0], scissorin.offset[1]},
				.extent = {scissorin.extent[0], scissorin.extent[1]},
				};
				vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
			},
			[this](const CmdCopyBufferToBuffer& arg) {
				VkBufferCopy bufferCopyData{
				   .srcOffset = arg.from.offset,
				   .dstOffset = arg.to.offset,
				   .size = arg.size
				};
				auto fromBuffer = std::static_pointer_cast<BufferVk>(arg.from.buffer);
				auto toBuffer = std::static_pointer_cast<BufferVk>(arg.to.buffer);
				vkCmdCopyBuffer(commandBuffer, fromBuffer->buffer, toBuffer->buffer, 1, &bufferCopyData);
			}
		};

		for (const auto& item : renderCommands) {
			std::visit(visitor, item);
		}

		renderCommands.clear();
	}
	bool CommandBufferVk::IsBufferSlotWritable(uint32_t slot)
	{
		auto findWritten = [&](auto&& bindingStore, bool& out) {
			if (!bindingStore) {
				out = false;
				return;
			}

			auto it = bindingStore->find(slot);
			if (it != bindingStore->end()) {
				out = it->second.writable;
			}
			else {
				out = false;
			}
		};

		if (currentRenderPipeline) {
			bool vsWritable = false, fsWritable = false;
			
			findWritten(currentRenderPipeline->vsBufferBindings, vsWritable);
			findWritten(currentRenderPipeline->fsBufferBindings, fsWritable);

			return vsWritable || fsWritable;
		}
		else if (currentComputePipeline) {
			bool isWritable = false;
			findWritten(currentComputePipeline->bufferBindings, isWritable);
			return isWritable;
		}
		else {
			Assert(false, "Invalid state!");
		}

	}
	void CommandBufferVk::ApplyBarriers()
	{
		if (barriersToAdd.size() == 0) {
			return;
		}

		VkDependencyInfo depInfo{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.pNext = nullptr,
			.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
			.memoryBarrierCount = 0,
			.pMemoryBarriers = nullptr,
			.bufferMemoryBarrierCount = uint32_t(barriersToAdd.size()),
			.pBufferMemoryBarriers = barriersToAdd.data(),
			.imageMemoryBarrierCount = 0,
			.pImageMemoryBarriers = nullptr
		};
		vkCmdPipelineBarrier2(
			commandBuffer,
			&depInfo
		);
		barriersToAdd.clear();
	}
}

#endif
