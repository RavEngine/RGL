#if RGL_DX12_AVAILABLE
#include "D3D12RenderPipeline.hpp"
#include "D3D12Device.hpp"

namespace RGL {
	PipelineLayoutD3D12::PipelineLayoutD3D12(decltype(owningDevice) owningDevice, const PipelineLayoutDescriptor& desc) : owningDevice(owningDevice)
	{
        
	}
    void PipelineLayoutD3D12::SetLayout(const LayoutConfig& config)
    {
        auto device = owningDevice->device;

        // Create a root signature.
        // version 1.1 is preferred, but we fall back to 1.0 if it is not available
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        // Allow input layout and deny unnecessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

        // A single 32-bit constant root parameter that is used by the vertex shader.
        const auto nconstants = config.constants.size();
        stackarray(rootParameters, CD3DX12_ROOT_PARAMETER1, nconstants);
        for (int i = 0; i < nconstants; i++) {
            rootParameters[i].InitAsConstants(config.constants[i].size_bytes / sizeof(int), config.constants[i].n_register, 0, D3D12_SHADER_VISIBILITY_ALL);
        }

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
        rootSignatureDescription.Init_1_1(nconstants, rootParameters, 0, nullptr, rootSignatureFlags);

        // Serialize the root signature.
        // it becomes a binary object which can be used to create the actual root signature
        ComPtr<ID3DBlob> rootSignatureBlob;
        ComPtr<ID3DBlob> errorBlob;
        DX_CHECK(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, featureData.HighestVersion, &rootSignatureBlob, &errorBlob));
        // Create the root signature.
        DX_CHECK(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
    }
}
#endif