#pragma once
#include <RGL/Types.hpp>
#include <RGL/CommandBuffer.hpp>
#include <vulkan/vulkan.h>
#include "RGLVk.hpp"
#include <unordered_set>
#include <unordered_map>

namespace RGL {
	struct DeviceVk;
	struct CommandQueueVk;
	struct RenderPipelineVk;
	struct RenderPassVk;

	struct CommandBufferVk : public ICommandBuffer {
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;	// does not need to be destroyed
		std::shared_ptr<RenderPassVk> currentRenderPass = nullptr;
		
		const std::shared_ptr<CommandQueueVk> owningQueue;
		std::shared_ptr<RenderPipelineVk> currentRenderPipeline = nullptr;
		std::shared_ptr<struct ComputePipelineVk> currentComputePipeline = nullptr;

		std::unordered_set<struct SwapchainVK*> swapchainsToSignal;

		struct TextureLastUse {
			VkImageLayout lastLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			bool written = false;
		};

		struct BufferLastUse {
			bool written = false;
		};

		std::unordered_map<const struct TextureVk*, TextureLastUse> activeTextures;
		std::unordered_set<const struct BufferVk*, BufferLastUse> activeBuffers;

		struct CmdSetVertexBuffer {

		};
		struct CmdSetIndexBuffer {

		};

		struct CmdBindRenderPipeline {

		};

		struct CmdBindRenderBuffer {

		};

		struct CmdSetPushConstantData {

		};
		
		struct CmdSetSampler {

		};

		struct CmdSetTexture {

		};

		struct CmdDraw {

		};

		struct CmdDrawIndexed {

		};

		struct CmdExecuteIndirect {

		};

		struct CmdExecuteIndirectIndexed {

		};

		struct CmdDebugMarker {

		};

		struct CmdBegin {

		};

		std::vector<std::variant<
			CmdSetVertexBuffer, 
			CmdSetIndexBuffer,
			CmdBindRenderPipeline,
			CmdBindRenderBuffer,
			CmdSetPushConstantData,
			CmdSetSampler,
			CmdSetTexture,
			CmdDraw,
			CmdDrawIndexed,
			CmdExecuteIndirect,
			CmdExecuteIndirectIndexed,
			CmdDebugMarker,
			CmdBegin
			>
		> renderCommands;

		CommandBufferVk(decltype(owningQueue) owningQueue);

		~CommandBufferVk();

		// ICommandBuffer
		void Reset() final;
		void Begin() final;
		void End() final;
		void BindRenderPipeline(RGLRenderPipelinePtr) final;

		void BeginRendering(RGLRenderPassPtr) final;
		void EndRendering() final;

		void BeginCompute(RGLComputePipelinePtr) final;
		void EndCompute() final;
		void DispatchCompute(uint32_t threadsX, uint32_t threadsY, uint32_t threadsZ, uint32_t threadsPerThreadgroupX=1, uint32_t threadsPerThreadgroupY=1, uint32_t threadsPerThreadgroupZ=1) final;

		void BindBuffer(RGLBufferPtr buffer, uint32_t bindingOffset, uint32_t offsetIntoBuffer = 0) final;

		void BindComputeBuffer(RGLBufferPtr buffer, uint32_t binding, uint32_t offsetIntoBuffer = 0) final;
		void SetVertexBuffer(RGLBufferPtr buffer, const VertexBufferBinding& bindingInfo = {}) final;

		void setPushConstantData(const RGL::untyped_span& data, const uint32_t& offset, VkShaderStageFlags stages);

		void SetVertexBytes(const untyped_span data, uint32_t offset) final;
		void SetFragmentBytes(const untyped_span data, uint32_t offset) final;
		void SetComputeBytes(const untyped_span data, uint32_t offset) final;

		void SetIndexBuffer(RGLBufferPtr buffer) final;

		void SetVertexSampler(RGLSamplerPtr sampler, uint32_t index) final;
		void SetFragmentSampler(RGLSamplerPtr sampler, uint32_t index) final;

		void SetVertexTexture(const ITexture* texture, uint32_t index) final;
		void SetFragmentTexture(const ITexture* texture, uint32_t index) final;

		void Draw(uint32_t nVertices, const DrawInstancedConfig & = {}) final;
		void DrawIndexed(uint32_t nIndices, const DrawIndexedInstancedConfig & = {}) final;

		void TransitionResource(const ITexture* texture, RGL::ResourceLayout current, RGL::ResourceLayout target, TransitionPosition position) final;
		void TransitionResources(std::initializer_list<ResourceTransition> transitions, TransitionPosition position) final;

		void CopyTextureToBuffer(RGL::ITexture* sourceTexture, const Rect& sourceRect, size_t offset, RGLBufferPtr destBuffer) final;
		void CopyBufferToBuffer(BufferCopyConfig from, BufferCopyConfig to, uint32_t size) final;

		void SetViewport(const Viewport&) final;
		void SetScissor(const Rect&) final;

		void Commit(const CommitConfig&) final;

		void SetResourceBarrier(const ResourceBarrierConfig&) final;
		void SetRenderPipelineBarrier(const PipelineBarrierConfig&) final;

		void ExecuteIndirectIndexed(const IndirectConfig&) final;
		void ExecuteIndirect(const IndirectConfig&) final;

		void BeginRenderDebugMarker(const std::string& label) final;
		void BeginComputeDebugMarker(const std::string& label) final;

		void EndRenderDebugMarker() final;
		void EndComputeDebugMarker() final;

	private:
		void GenericBindBuffer(RGLBufferPtr& buffer, const uint32_t& offsetIntoBuffer, const uint32_t& bindingOffset, VkPipelineBindPoint bindPoint);
		void RecordBufferBinding(const BufferVk* buffer, BufferLastUse usage);
		void RecordTextureBinding(const TextureVk texture, TextureLastUse usage);
	};
}
