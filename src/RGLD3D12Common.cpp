#if RGL_DX12_AVAILABLE
#include "Types.hpp"
#include "RGLD3D12.hpp"
#include "RGLCommon.hpp"
#include "D3D12RenderPass.hpp"
#include <d3d12sdklayers.h>
#include <wrl.h>

using namespace RGL;
using namespace Microsoft::WRL;

namespace RGL {
    void EnableDebugLayer()
    {
#if defined(_DEBUG)
        // Always enable the debug layer before doing anything DX12 related
        // so all possible errors generated while creating DX12 objects
        // are caught by the debug layer.
        // Enabling the debug layer after creating the ID3D12Device will cause the runtime to remove the device. 
        ComPtr<ID3D12Debug> debugInterface;
        DX_CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
        debugInterface->EnableDebugLayer();
#endif
    }

    void RGL::InitD3D12(const RGL::InitOptions& options) {
        Assert(CanInitAPI(RGL::API::Direct3D12), "Direct3D12 cannot be initialized on this platform.");
        RGL::currentAPI = API::Direct3D12;
        EnableDebugLayer();
    }

    void RGL::DeintD3D12()
    {
        // as of now, do nothing
    }

    ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device2> device,
        D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
    {
        ComPtr<ID3D12DescriptorHeap> descriptorHeap;

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = numDescriptors;
        desc.Type = type;

        DX_CHECK(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

        return descriptorHeap;
    }
    RGLRenderPassPtr CreateRenderPassD3D12(const RenderPassConfig& config)
    {
        return std::make_shared<RenderPassD3D12>(config);
    }
}
#endif
