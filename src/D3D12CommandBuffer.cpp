#if RGL_DX12_AVAILABLE
#include "D3D12CommandBuffer.hpp"
#include "D3D12CommandQueue.hpp"
#include "D3D12Synchronization.hpp"

namespace RGL {
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

	}
	void CommandBufferD3D12::End()
	{
		commandList->Close();
	}
	void CommandBufferD3D12::BeginRendering(const BeginRenderingConfig&)
	{
	}
	void CommandBufferD3D12::EndRendering()
	{
	}
	void CommandBufferD3D12::BindPipeline(std::shared_ptr<IRenderPipeline>)
	{
	}
	void CommandBufferD3D12::BindBuffer(std::shared_ptr<IBuffer> buffer, uint32_t offset)
	{
	}
	void CommandBufferD3D12::Draw(uint32_t nVertices, uint32_t nInstances, uint32_t startVertex, uint32_t firstInstance)
	{
	}
	void CommandBufferD3D12::SetViewport(const Viewport& viewport)
	{
	}
	void CommandBufferD3D12::SetScissor(const Scissor& scissor)
	{
		
	}
	void CommandBufferD3D12::Commit(const CommitConfig& config)
	{
		owningQueue->ExecuteCommandList(commandList);
		auto d3d12fence = std::static_pointer_cast<FenceD3D12>(config.signalFence);
		owningQueue->m_d3d12CommandQueue->Signal(d3d12fence->fence.Get(),1);	// 1 because we emulate binary vulkan fences
	}
}

#endif