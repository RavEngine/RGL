#if RGL_DX12_AVAILABLE
#include "D3D12CommandBuffer.hpp"
#include "D3D12CommandQueue.hpp"
#include "D3D12Synchronization.hpp"
#include <directx/d3dx12.h>
#include "D3D12Texture.hpp"
#include "D3D12RenderPipeline.hpp"
#include "D3D12Buffer.hpp"
#include "D3D12Device.hpp"
#include "D3D12Sampler.hpp"
#include "D3D12RenderPass.hpp"

namespace RGL {
	void TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			resource.Get(),
			beforeState, afterState);

		commandList->ResourceBarrier(1, &barrier);
	}


	CommandBufferD3D12::CommandBufferD3D12(decltype(owningQueue) owningQueue) : owningQueue(owningQueue)
	{
		commandList = owningQueue->CreateCommandList();
	}
	void CommandBufferD3D12::Reset()
	{
		ID3D12CommandAllocator* commandAllocator;
		UINT dataSize = sizeof(commandAllocator);
		DX_CHECK(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator));
		if (!ended) {
			DX_CHECK(commandList->Close());
		}
		DX_CHECK(commandAllocator->Reset());	// gotta reset this too, otherwise we leak
		DX_CHECK(commandList->Reset(commandAllocator, nullptr));
		ended = false;
	}
	void CommandBufferD3D12::Begin()
	{
		//TODO: read from the renderpipeline instead of hardcoding this
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	void CommandBufferD3D12::End()
	{
		DX_CHECK(commandList->Close());
		ended = true;
	}
	void CommandBufferD3D12::BeginRendering(RGLRenderPassPtr renderPass)
	{
		currentRenderPass = std::static_pointer_cast<RenderPassD3D12>(renderPass);

		const auto nrtvs = currentRenderPass->config.attachments.size();
		stackarray(rtvs, CD3DX12_CPU_DESCRIPTOR_HANDLE,nrtvs);

		uint32_t i = 0;
		for (const auto& attachment : currentRenderPass->config.attachments) {
			auto tx = static_cast<TextureD3D12*>(currentRenderPass->textures[i]);

			if (attachment.shouldTransition) {
				TransitionResource(commandList, tx->texture,
					D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			}
			auto rtv = CD3DX12_CPU_DESCRIPTOR_HANDLE(tx->owningDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
				tx->descriptorHeapOffset, tx->owningDevice->g_RTVDescriptorHeapSize);
			commandList->ClearRenderTargetView(rtv, attachment.clearColor.data(), 0, nullptr);

			rtvs[i] = rtv;

			i++;
		}

		//depth stencil
		D3D12_CPU_DESCRIPTOR_HANDLE dsv;
		D3D12_CPU_DESCRIPTOR_HANDLE* dsvptr = nullptr;
		{
			if (currentRenderPass->depthTexture) {
				auto tx = static_cast<TextureD3D12*>(currentRenderPass->depthTexture);
				if (currentRenderPass->config.depthAttachment->shouldTransition) {
					TransitionResource(commandList, tx->texture,
						D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
				}
				dsv = tx->owningDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				dsvptr = &dsv;
				commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, currentRenderPass->config.depthAttachment->clearColor[0], 0, 0, nullptr);
			}
		}

		// bind the targets
		commandList->OMSetRenderTargets(nrtvs, rtvs, FALSE, dsvptr);
	}
	void CommandBufferD3D12::EndRendering()
	{
		uint32_t i = 0;
		for (const auto& attachment : currentRenderPass->config.attachments) {
			auto tx = static_cast<TextureD3D12*>(currentRenderPass->textures[i]);
			if (attachment.shouldTransition) {
				TransitionResource(commandList, tx->texture,
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			}
		}

		currentRenderPass = nullptr;
	}
	void CommandBufferD3D12::BindPipeline(RGLRenderPipelinePtr in_pipeline)
	{
		auto pipeline = std::static_pointer_cast<RenderPipelineD3D12>(in_pipeline);
		commandList->SetPipelineState(pipeline->pipelineState.Get());
		commandList->SetGraphicsRootSignature(pipeline->pipelineLayout->rootSignature.Get());
	}
	void CommandBufferD3D12::BindBuffer(RGLBufferPtr buffer, uint32_t offset)
	{
		commandList->IASetVertexBuffers(0, 1, &std::static_pointer_cast<BufferD3D12>(buffer)->vertexBufferView);
	}
	void CommandBufferD3D12::SetVertexBytes(const untyped_span data, uint32_t offset)
	{
		auto size = data.size() / 4 + (data.size() % 4 != 0 ? 1 : 0);
		commandList->SetGraphicsRoot32BitConstants(offset, size, data.data(), 0);
	}
	void CommandBufferD3D12::SetFragmentBytes(const untyped_span data, uint32_t offset)
	{

	}
	void CommandBufferD3D12::SetIndexBuffer(RGLBufferPtr buffer)
	{
		commandList->IASetIndexBuffer(&(std::static_pointer_cast<BufferD3D12>(buffer)->indexBufferView));
	}
	void CommandBufferD3D12::SetVertexSampler(RGLSamplerPtr sampler, uint32_t index)
	{
		//commandList->SetGraphicsRootDescriptorTable(index, std::static_pointer_cast<SamplerD3D12>(sampler)->descHandle);
		// https://stackoverflow.com/questions/55628161/how-to-bind-textures-to-different-register-in-dx12
	}
	void CommandBufferD3D12::SetFragmentSampler(RGLSamplerPtr sampler, uint32_t index)
	{
	}
	void CommandBufferD3D12::SetVertexTexture(const ITexture* texture, uint32_t index)
	{

	}
	void CommandBufferD3D12::SetFragmentTexture(const ITexture* texture, uint32_t index)
	{
	}
	void CommandBufferD3D12::Draw(uint32_t nVertices, const DrawInstancedConfig& config)
	{
		commandList->DrawInstanced(nVertices, config.nInstances, config.startVertex, config.firstInstance);
	}
	void CommandBufferD3D12::DrawIndexed(uint32_t nIndices, const DrawIndexedInstancedConfig& config)
	{
		commandList->DrawIndexedInstanced(nIndices, config.nInstances, config.firstIndex, config.startVertex, config.firstInstance);
	}
	void CommandBufferD3D12::SetViewport(const Viewport& viewport)
	{
		D3D12_VIEWPORT m_Viewport{
			.TopLeftX = viewport.x,
			.TopLeftY = viewport.y,
			.Width = viewport.width,
			.Height = viewport.height,
			.MinDepth = viewport.minDepth,
			.MaxDepth = viewport.maxDepth,
		};

		commandList->RSSetViewports(1, &m_Viewport);
	}
	void CommandBufferD3D12::SetScissor(const Scissor& scissor)
	{
		D3D12_RECT m_ScissorRect{ CD3DX12_RECT(scissor.offset[0], scissor.offset[1], scissor.extent[0], scissor.extent[1])};
		commandList->RSSetScissorRects(1, &m_ScissorRect);
	}
	void CommandBufferD3D12::Commit(const CommitConfig& config)
	{
		owningQueue->ExecuteCommandList(commandList);
		auto d3d12fence = std::static_pointer_cast<FenceD3D12>(config.signalFence);
		owningQueue->m_d3d12CommandQueue->Signal(d3d12fence->fence.Get(),1);	// 1 because we emulate binary vulkan fences
	}
}

#endif