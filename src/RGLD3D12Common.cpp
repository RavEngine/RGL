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
        D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
    {
        ComPtr<ID3D12DescriptorHeap> descriptorHeap;

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = numDescriptors;
        desc.Type = type;
        desc.Flags = flags;

        DX_CHECK(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

        return descriptorHeap;
    }
    RGLRenderPassPtr CreateRenderPassD3D12(const RenderPassConfig& config)
    {
        return std::make_shared<RenderPassD3D12>(config);
    }

    DXGI_FORMAT rgl2dxgiformat_texture(RGL::TextureFormat format) {
        switch (format) {
        case decltype(format)::BGRA8_Unorm:  return DXGI_FORMAT_R8G8B8A8_UNORM;
        case decltype(format)::RGBA8_Unorm:  return DXGI_FORMAT_R8G8B8A8_UNORM;
        case decltype(format)::RGBA8_Uint:  return DXGI_FORMAT_R8G8B8A8_UINT;

        case decltype(format)::D32SFloat:  return DXGI_FORMAT_D32_FLOAT;
        case decltype(format)::D24UnormS8Uint:  return DXGI_FORMAT_D24_UNORM_S8_UINT;


        case decltype(format)::Undefined:  return DXGI_FORMAT_UNKNOWN;
        default:
            FatalError("Unsupported texture format");
        }
    }
}
#endif
