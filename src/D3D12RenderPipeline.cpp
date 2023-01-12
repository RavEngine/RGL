#if RGL_DX12_AVAILABLE
#include "D3D12RenderPipeline.hpp"
#include "D3D12Device.hpp"
#include "D3D12ShaderLibrary.hpp"

namespace RGL {
    DXGI_FORMAT rgl2dxgiformat(RenderPipelineDescriptor::VertexConfig::VertexAttributeDesc::Format format) {
        switch (format) {
        case decltype(format)::R32G32B32_SignedFloat:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case decltype(format)::R32G32_SignedFloat:
            return DXGI_FORMAT_R32G32_FLOAT;
        default:
            FatalError("Unsupported vertex attribute format");
        }
    }

    DXGI_FORMAT rgl2dxgiformat_texture(RGL::TextureFormat format) {
        switch (format) {
        case decltype(format)::BGRA8_Unorm:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        default:
            FatalError("Unsupported texture format");
        }
    }

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


    RenderPipelineD3D12::RenderPipelineD3D12(decltype(owningDevice) owningDevice, const RenderPipelineDescriptor& desc) : owningDevice(owningDevice)
    {
        auto device = owningDevice->device;
        struct PipelineStateStream
        {
            CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
            CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
            CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
            CD3DX12_PIPELINE_STATE_STREAM_VS VS;
            CD3DX12_PIPELINE_STATE_STREAM_PS PS;
            CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
            CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
        } pipelineStateStream;
        
        const auto nattributes = desc.vertexConfig.attributeDescs.size();

        // create the vertex attribute layout
        stackarray(inputLayout, D3D12_INPUT_ELEMENT_DESC, nattributes);
        for (UINT i = 0; i < nattributes; i++) {
            auto& attr = desc.vertexConfig.attributeDescs[i];
            inputLayout[i] = { attr.semantic_name, i, rgl2dxgiformat(attr.format), 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0};
        }

        // setup the shaders
        std::shared_ptr<ShaderLibraryD3D12> vertFunc, fragFunc;
        for (const auto& func : desc.stages) {
            switch (func.type) {
            case RenderPipelineDescriptor::ShaderStageDesc::Type::Vertex:
                vertFunc = std::static_pointer_cast<decltype(vertFunc)::element_type>(func.shaderModule);
                break;
            case RenderPipelineDescriptor::ShaderStageDesc::Type::Fragment:
                fragFunc = std::static_pointer_cast<decltype(vertFunc)::element_type>(func.shaderModule);
                break;
            default:
                FatalError("Function type is not supported");
            }
        }

        // set the render target texture formats
        const auto nattachments = desc.colorBlendConfig.attachments.size();

        D3D12_RT_FORMAT_ARRAY rtvFormats = {
            .NumRenderTargets = static_cast<UINT>(nattachments)
        };
        Assert(nattachments < __crt_countof(rtvFormats.RTFormats), "Too many attachments!");
        for (int i = 0; i < nattachments; i++) {
            rtvFormats.RTFormats[0] = rgl2dxgiformat_texture(desc.colorBlendConfig.attachments[i].format);
        }

        // describe the pipeline state object
        pipelineStateStream.pRootSignature = std::static_pointer_cast<PipelineLayoutD3D12>(desc.pipelineLayout)->rootSignature.Get();
        pipelineStateStream.InputLayout = { inputLayout, static_cast<uint32_t>(nattributes) };
        pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pipelineStateStream.VS = vertFunc->shaderBytecode;
        pipelineStateStream.PS = fragFunc->shaderBytecode;
        pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        pipelineStateStream.RTVFormats = rtvFormats;

        // create the PSO
        D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
           sizeof(PipelineStateStream), &pipelineStateStream
        };
        DX_CHECK(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&pipelineState)));
    }
}
#endif