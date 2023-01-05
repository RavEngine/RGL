#pragma once
#include "CommandQueue.hpp"

#include <d3d12.h>  // For ID3D12CommandQueue, ID3D12Device2, and ID3D12Fence
#include <wrl.h>    // For Microsoft::WRL::ComPtr

#include <cstdint>  // For uint64_t
#include <queue>    // For std::queue

namespace RGL {
    struct DeviceD3D12;
    
	struct CommandQueueD3D12 : public ICommandQueue {
		std::shared_ptr<ICommandBuffer> CreateCommandBuffer() final;

        CommandQueueD3D12(Microsoft::WRL::ComPtr<ID3D12Device2> device, QueueType type);
        virtual ~CommandQueueD3D12() {}

        // Get an available command list from the command queue.
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> GetCommandList();

        // Execute a command list.
        // Returns the fence value to wait for for this command list.
        uint64_t ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);

        uint64_t Signal();
        bool IsFenceComplete(uint64_t fenceValue);
        void WaitForFenceValue(uint64_t fenceValue);
        void Flush();

        Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const {
            return m_d3d12CommandQueue;
        }

    protected:

        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator);

    private:
        // Keep track of command allocators that are "in-flight"
        struct CommandAllocatorEntry
        {
            uint64_t fenceValue;
            Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator; // must have one for each frame in flight (at least the number of swapchain images)
        };

        using CommandAllocatorQueue = std::queue<CommandAllocatorEntry>;
        using CommandListQueue = std::queue<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> >;

        D3D12_COMMAND_LIST_TYPE m_CommandListType;
        Microsoft::WRL::ComPtr<ID3D12Device2> m_d3d12Device;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue;
        Microsoft::WRL::ComPtr<ID3D12Fence> m_d3d12Fence;

        HANDLE  m_FenceEvent;
        uint64_t m_FenceValue;

        CommandAllocatorQueue m_CommandAllocatorQueue;
        CommandListQueue m_CommandListQueue;
	};

}