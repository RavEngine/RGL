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

	D3D12_RESOURCE_STATES rgl2d3d12resourcestate(RGL::ResourceLayout layout) {
		switch (layout) {
			case decltype(layout)::Undefined:
			case decltype(layout)::General: 
			case decltype(layout)::Reinitialized:
				return D3D12_RESOURCE_STATE_COMMON;

			case decltype(layout)::ColorAttachmentOptimal: 
			case decltype(layout)::DepthStencilAttachmentOptimal:
			case decltype(layout)::DepthAttachmentOptimal:
			case decltype(layout)::StencilAttachmentOptimal:
			case decltype(layout)::AttachmentOptimal:
				return D3D12_RESOURCE_STATE_RENDER_TARGET;

			case decltype(layout)::DepthStencilReadOnlyOptimal: 
				return D3D12_RESOURCE_STATE_DEPTH_READ;

			case decltype(layout)::ShaderReadOnlyOptimal: 
				return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

			case decltype(layout)::TransferSourceOptimal: 
				return D3D12_RESOURCE_STATE_COPY_SOURCE;

			case decltype(layout)::TransferDestinationOptimal: 
				return D3D12_RESOURCE_STATE_COPY_DEST;

			case decltype(layout)::DepthReadOnlyStencilAttachmentOptimal: 
			case decltype(layout)::DepthAttachmentStencilReadOnlyOptimal:
				return D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_RENDER_TARGET;

			case decltype(layout)::DepthReadOnlyOptimal: 
			case decltype(layout)::StencilReadOnlyOptimal:
				return D3D12_RESOURCE_STATE_DEPTH_READ;

			case decltype(layout)::ReadOnlyOptimal:
				return D3D12_RESOURCE_STATE_GENERIC_READ;

			case decltype(layout)::Present: 
				return D3D12_RESOURCE_STATE_PRESENT;

			default:
				FatalError("layout is not supported");
		}
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

			if (attachment.preTransition) {
				const auto fromState = rgl2d3d12resourcestate(attachment.preTransition->beforeLayout);
				const auto toState = rgl2d3d12resourcestate(attachment.preTransition->afterLayout);
				TransitionResource(commandList, tx->texture,
					fromState, toState);
			}
			Assert(tx->rtvAllocated(),"This texture was not allocated as a render target!");
			
			auto rtv = tx->owningDevice->RTVHeap->GetCpuHandle(tx->rtvIDX);

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
				if (currentRenderPass->config.depthAttachment->preTransition) {
					const auto fromState = rgl2d3d12resourcestate(currentRenderPass->config.depthAttachment->preTransition->beforeLayout);
					const auto toState = rgl2d3d12resourcestate(currentRenderPass->config.depthAttachment->preTransition->afterLayout);
					TransitionResource(commandList, tx->texture,
						D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
				}
				Assert(tx->dsvAllocated(), "Texture was not allocated as a depth stencil!");
				dsv = tx->owningDevice->DSVHeap->GetCpuHandle(tx->dsvIDX);
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
			if (attachment.postTransition) {
				const auto fromState = rgl2d3d12resourcestate(attachment.postTransition->beforeLayout);
				const auto toState = rgl2d3d12resourcestate(attachment.postTransition->afterLayout);
				TransitionResource(commandList, tx->texture,
					fromState, toState);
			}
		}

		//depth stencil
		{
			if (currentRenderPass->depthTexture) {
				auto tx = static_cast<TextureD3D12*>(currentRenderPass->depthTexture);
				if (currentRenderPass->config.depthAttachment->postTransition) {
					const auto fromState = rgl2d3d12resourcestate(currentRenderPass->config.depthAttachment->postTransition->beforeLayout);
					const auto toState = rgl2d3d12resourcestate(currentRenderPass->config.depthAttachment->postTransition->afterLayout);
					TransitionResource(commandList, tx->texture,
						fromState, toState);
				}
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
	void CommandBufferD3D12::BindBuffer(RGLBufferPtr buffer, uint32_t bindingOffset, uint32_t offsetIntoBuffer)
	{
		auto casted = std::static_pointer_cast<BufferD3D12>(buffer);
		commandList->SetGraphicsRootShaderResourceView(bindingOffset + 1, casted->vertexBufferView.BufferLocation + offsetIntoBuffer);
	}
	void CommandBufferD3D12::SetVertexBuffer(RGLBufferPtr buffer, uint32_t offsetIntoBuffer)
	{
		commandList->IASetVertexBuffers(0, 1, &std::static_pointer_cast<BufferD3D12>(buffer)->vertexBufferView + offsetIntoBuffer);
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
		
	}
	void CommandBufferD3D12::SetFragmentSampler(RGLSamplerPtr sampler, uint32_t index)
	{
		index += 1;
		auto thisSampler = std::static_pointer_cast<SamplerD3D12>(sampler);
		auto& samplerHeap = thisSampler->owningDevice->SamplerHeap;
		ID3D12DescriptorHeap* heapForThis = samplerHeap->Heap();
		auto handle = samplerHeap->GetGpuHandle(thisSampler->descriptorIndex);
		commandList->SetDescriptorHeaps(1, &heapForThis);
		commandList->SetGraphicsRootDescriptorTable(index, handle);
	}
	void CommandBufferD3D12::SetVertexTexture(const ITexture* texture, uint32_t index)
	{

	}
	void CommandBufferD3D12::SetFragmentTexture(const ITexture* texture, uint32_t index)
	{
		index += 1;
		auto thisTexture = static_cast<const TextureD3D12*>(texture);
		auto& heapForThis = thisTexture->owningDevice->CBV_SRV_UAVHeap.value();
		auto ptr = heapForThis.Heap();
		commandList->SetDescriptorHeaps(1, &ptr);
		// bindings come in pairs (sampler, texture, sampler, texture)
		commandList->SetGraphicsRootDescriptorTable(index + 1, heapForThis.GetGpuHandle(thisTexture->srvIDX));
	}
	void CommandBufferD3D12::SetCombinedTextureSampler(RGLSamplerPtr sampler, const ITexture* texture, uint32_t index)
	{
		index += 1;
		auto thisSampler = std::static_pointer_cast<SamplerD3D12>(sampler);
		auto thisTexture = static_cast<const TextureD3D12*>(texture);
		auto& srvheap = thisTexture->owningDevice->CBV_SRV_UAVHeap;
		auto& samplerHeap = thisSampler->owningDevice->SamplerHeap;
		ID3D12DescriptorHeap* heapForThis[2] = { samplerHeap->Heap(), srvheap->Heap()};
		commandList->SetDescriptorHeaps(std::size(heapForThis), heapForThis);
		commandList->SetGraphicsRootDescriptorTable(index, samplerHeap->GetGpuHandle(thisSampler->descriptorIndex));
		commandList->SetGraphicsRootDescriptorTable(index + 1, srvheap->GetGpuHandle(thisTexture->srvIDX));
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