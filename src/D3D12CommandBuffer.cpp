#if RGL_DX12_AVAILABLE
#include "D3D12CommandBuffer.hpp"
#include "D3D12CommandQueue.hpp"
#include "D3D12Synchronization.hpp"
#include <directx/d3dx12.h>
#include "D3D12Texture.hpp"
#include "D3D12RenderPipeline.hpp"
#include "D3D12Buffer.hpp"
#include "D3D12Device.hpp"

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
		commandList->Close();
		ID3D12CommandAllocator* commandAllocator;
		UINT dataSize = sizeof(commandAllocator);
		DX_CHECK(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator));
		commandList->Reset(commandAllocator, nullptr);
	}
	void CommandBufferD3D12::Begin()
	{
		//TODO: read from the renderpipeline instead of hardcoding this
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	void CommandBufferD3D12::End()
	{
		TransitionResource(commandList, currentBackbuffer->texture,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		commandList->Close();
	}
	void CommandBufferD3D12::BeginRendering(const BeginRenderingConfig& config)
	{
		auto tx = static_cast<TextureD3D12*>(config.targetFramebuffer);
		currentBackbuffer = tx;
		
		TransitionResource(commandList, tx->texture,
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		auto rtv = CD3DX12_CPU_DESCRIPTOR_HANDLE(tx->owningDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			tx->descriptorHeapOffset, tx->owningDevice->g_RTVDescriptorHeapSize);
		commandList->ClearRenderTargetView(rtv, config.clearColor.data(), 0, nullptr);

		//TODO: allow passing a depth stencil
		commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

		//commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
	}
	void CommandBufferD3D12::EndRendering()
	{
	}
	void CommandBufferD3D12::BindPipeline(std::shared_ptr<IRenderPipeline> in_pipeline)
	{
		auto pipeline = std::static_pointer_cast<RenderPipelineD3D12>(in_pipeline);
		commandList->SetPipelineState(pipeline->pipelineState.Get());
		commandList->SetGraphicsRootSignature(pipeline->pipelineLayout->rootSignature.Get());
	}
	void CommandBufferD3D12::BindBuffer(std::shared_ptr<IBuffer> buffer, uint32_t offset)
	{
		commandList->IASetVertexBuffers(0, 1, &std::static_pointer_cast<BufferD3D12>(buffer)->bufferView);
	}
	void CommandBufferD3D12::Draw(uint32_t nVertices, uint32_t nInstances, uint32_t startVertex, uint32_t firstInstance)
	{
		commandList->DrawInstanced(nVertices, nInstances, startVertex, firstInstance);
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