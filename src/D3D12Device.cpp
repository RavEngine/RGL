#if RGL_DX12_AVAILABLE
#include "D3D12Device.hpp"
#include "RGLD3D12.hpp"
#include "RGLCommon.hpp"
#include <codecvt>

using namespace RGL;
using namespace Microsoft::WRL;

ComPtr<IDXGIAdapter4> GetAdapter(bool useWarp)
{
    ComPtr<IDXGIFactory4> dxgiFactory;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    DX_CHECK(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

    ComPtr<IDXGIAdapter1> dxgiAdapter1;
    ComPtr<IDXGIAdapter4> dxgiAdapter4;

    // the software renderer
    if (useWarp)
    {
        DX_CHECK(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
        DX_CHECK(dxgiAdapter1.As(&dxgiAdapter4));
    }
    else
    {
        SIZE_T maxDedicatedVideoMemory = 0;
        for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

            // Check to see if the adapter can create a D3D12 device without actually 
            // creating it. The adapter with the largest dedicated video memory
            // is favored.
            // use DXGI_ADAPTER_FLAG_SOFTWARE to only select hardware devices
            if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
                    D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
                dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
            {
                maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                DX_CHECK(dxgiAdapter1.As(&dxgiAdapter4));
            }
        }
    }

    return dxgiAdapter4;
}

// create device from adapter
// destroying a device causes all resources allocated on it to become invalid
// should be destroyed after all resources have been destroyed (the validation layer will complain otherwise)
ComPtr<ID3D12Device2> CreateDevice(ComPtr<IDXGIAdapter4> adapter)
{
    ComPtr<ID3D12Device2> d3d12Device2;
    DX_CHECK(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device2)));

    // Enable debug messages in debug mode.
#if defined(_DEBUG)
    ComPtr<ID3D12InfoQueue> pInfoQueue;
    if (SUCCEEDED(d3d12Device2.As(&pInfoQueue)))
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
        // Suppress whole categories of messages
        // uncomment to provide some
        //D3D12_MESSAGE_CATEGORY Categories[] = {};

        // Suppress messages based on their severity level
        D3D12_MESSAGE_SEVERITY Severities[] =
        {
            D3D12_MESSAGE_SEVERITY_INFO     // these don't indicate misuse of the API so we ignore them
        };

        // Suppress individual messages by their ID
        D3D12_MESSAGE_ID DenyIds[] = {
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message. Happens when clearing using a color other than the optimized clear color. If arbitrary clear colors are not desired, dont' suppress this message
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
        };

        D3D12_INFO_QUEUE_FILTER NewFilter = {};
        //NewFilter.DenyList.NumCategories = _countof(Categories);
        //NewFilter.DenyList.pCategoryList = Categories;
        NewFilter.DenyList.NumSeverities = _countof(Severities);
        NewFilter.DenyList.pSeverityList = Severities;
        NewFilter.DenyList.NumIDs = _countof(DenyIds);
        NewFilter.DenyList.pIDList = DenyIds;

        DX_CHECK(pInfoQueue->PushStorageFilter(&NewFilter));
    }
#endif

    return d3d12Device2;
}


std::shared_ptr<IDevice> RGL::CreateDefaultDeviceD3D12() {
    ComPtr<IDXGIAdapter4> dxgiAdapter4 = GetAdapter(false); // for now, don't use WARP
	return std::make_shared<DeviceD3D12>(dxgiAdapter4);
}

DeviceD3D12::DeviceD3D12(decltype(adapter) adapter) : adapter(adapter), device(CreateDevice(adapter)) {
	// TODO: create commandqueues
}

DeviceD3D12::~DeviceD3D12() {
    // all comptrs decrement to 0 if not held elsewhere and are released
}

std::string DeviceD3D12::GetBrandString() {
    DXGI_ADAPTER_DESC desc;
    adapter->GetDesc(&desc);

    std::wstring wstr(desc.Description);

    //setup converter
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.to_bytes(wstr);
}
std::shared_ptr<ISwapchain> RGL::DeviceD3D12::CreateSwapchain(std::shared_ptr<ISurface>, int width, int height)
{
    FatalError("Not implemented");
    return std::shared_ptr<ISwapchain>();
}
std::shared_ptr<IRenderPass> RGL::DeviceD3D12::CreateRenderPass(const RenderPassConfig&)
{
    FatalError("Not implemented");
    return std::shared_ptr<IRenderPass>();
}
std::shared_ptr<IPipelineLayout> RGL::DeviceD3D12::CreatePipelineLayout(const PipelineLayoutDescriptor&)
{
    FatalError("Not implemented");
    return std::shared_ptr<IPipelineLayout>();
}
std::shared_ptr<IRenderPipeline> RGL::DeviceD3D12::CreateRenderPipeline(const std::shared_ptr<IPipelineLayout>, const std::shared_ptr<IRenderPass>, const RenderPipelineDescriptor&)
{
    FatalError("Not implemented");
    return std::shared_ptr<IRenderPipeline>();
}
std::shared_ptr<IShaderLibrary> RGL::DeviceD3D12::CreateDefaultShaderLibrary()
{
    FatalError("Not implemented");
    return std::shared_ptr<IShaderLibrary>();
}
std::shared_ptr<IShaderLibrary> RGL::DeviceD3D12::CreateShaderLibraryFromBytes(const std::span<uint8_t>)
{
    FatalError("Not implemented");
    return std::shared_ptr<IShaderLibrary>();
}
std::shared_ptr<IShaderLibrary> RGL::DeviceD3D12::CreateShaderLibrarySourceCode(const std::string_view)
{
    FatalError("Not implemented");
    return std::shared_ptr<IShaderLibrary>();
}
std::shared_ptr<IShaderLibrary> RGL::DeviceD3D12::CreateShaderLibraryFromPath(const std::filesystem::path&)
{
    FatalError("Not implemented");
    return std::shared_ptr<IShaderLibrary>();
}

std::shared_ptr<IBuffer> RGL::DeviceD3D12::CreateBuffer(const BufferConfig&)
{
    FatalError("Not implemented");
    return std::shared_ptr<IBuffer>();
}

#endif
