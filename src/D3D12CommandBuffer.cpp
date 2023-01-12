#if RGL_DX12_AVAILABLE
#include "D3D12CommandBuffer.hpp"
#include "D3D12CommandQueue.hpp"

namespace RGL {
	CommandBufferD3D12::CommandBufferD3D12(decltype(owningQueue) owningQueue) : owningQueue(owningQueue)
	{
		auto pair = owningQueue->GetCommandList();
		commandList = pair.list;
		commandAllocator = pair.commandAllocator;
	}
	void CommandBufferD3D12::Reset()
	{
		commandList->Reset(commandAllocator.Get(),nullptr);
	}
	void CommandBufferD3D12::Begin()
	{
	}
	void CommandBufferD3D12::End()
	{
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

	}
}

#endif