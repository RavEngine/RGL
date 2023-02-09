#if RGL_DX12_AVAILABLE
#include "D3D12RenderPipeline.hpp"
#include "D3D12Device.hpp"
#include "D3D12ShaderLibrary.hpp"
#include "D3D12Sampler.hpp"

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

    D3D12_CULL_MODE rgl2d3d12cull(RGL::RenderPipelineDescriptor::RasterizerConfig::CullMode cull) {
        switch (cull) {
        case decltype(cull)::None:
            return D3D12_CULL_MODE_NONE;
        case decltype(cull)::Front:
            return D3D12_CULL_MODE_FRONT;
        case decltype(cull)::Back:
            return D3D12_CULL_MODE_BACK;
        default:
            return D3D12_CULL_MODE_NONE;    //TODO: this should be the All option which results in no rendering
        }
    };

	PipelineLayoutD3D12::PipelineLayoutD3D12(decltype(owningDevice) owningDevice, const PipelineLayoutDescriptor& desc) : owningDevice(owningDevice)
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

        // create the constants data
        const auto nconstants = desc.constants.size();
        stackarray(rootParameters, CD3DX12_ROOT_PARAMETER1, nconstants);
        for (int i = 0; i < nconstants; i++) {
            rootParameters[i].InitAsConstants(desc.constants[i].size_bytes / sizeof(int), desc.constants[i].n_register, 0, D3D12_SHADER_VISIBILITY_ALL);
        }

        const auto numSamplers = desc.boundSamplers.size();
        stackarray(samplerStates, D3D12_STATIC_SAMPLER_DESC, numSamplers);
        {
            uint32_t i = 0;
            for (const auto& isampler : desc.boundSamplers) {
                auto sampler = std::static_pointer_cast<SamplerD3D12>(isampler);
                samplerStates[i] = {
                    //TODO: fill
                };
            }
        }


        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
        rootSignatureDescription.Init_1_1(nconstants, rootParameters, numSamplers, samplerStates, rootSignatureFlags);

        // Serialize the root signature.
        // it becomes a binary object which can be used to create the actual root signature
        ComPtr<ID3DBlob> rootSignatureBlob;
        ComPtr<ID3DBlob> errorBlob;
        DX_CHECK(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, featureData.HighestVersion, &rootSignatureBlob, &errorBlob));
        // Create the root signature.
        DX_CHECK(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
	}
    void PipelineLayoutD3D12::SetLayout(const LayoutConfig& config)
    {
        // currently does nothing...
    }


    RenderPipelineD3D12::RenderPipelineD3D12(decltype(owningDevice) owningDevice, const RenderPipelineDescriptor& desc) : owningDevice(owningDevice), pipelineLayout(std::static_pointer_cast<PipelineLayoutD3D12>(desc.pipelineLayout))
    {
        auto device = owningDevice->device;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
        
        const auto nattributes = desc.vertexConfig.attributeDescs.size();

        // create the vertex attribute layout
        stackarray(inputLayout, D3D12_INPUT_ELEMENT_DESC, nattributes);
        for (UINT i = 0; i < nattributes; i++) {
            auto& attr = desc.vertexConfig.attributeDescs[i];
            inputLayout[i] = { "TEXCOORD", i, rgl2dxgiformat(attr.format), 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 };
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

        pipelineStateDesc.NumRenderTargets = static_cast<UINT>(nattachments);

        Assert(nattachments < __crt_countof(pipelineStateDesc.RTVFormats), "Too many attachments!");
        for (int i = 0; i < nattachments; i++) {
            pipelineStateDesc.RTVFormats[0] = rgl2dxgiformat_texture(desc.colorBlendConfig.attachments[i].format);
        }

        CD3DX12_RASTERIZER_DESC rasterizerDesc{ D3D12_DEFAULT };
        rasterizerDesc.CullMode = rgl2d3d12cull(desc.rasterizerConfig.cullMode);
        rasterizerDesc.FrontCounterClockwise = desc.rasterizerConfig.windingOrder == decltype(desc.rasterizerConfig.windingOrder)::Counterclockwise;

        CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc{ D3D12_DEFAULT };
        depthStencilDesc.DepthEnable = false;
        depthStencilDesc.StencilEnable = false;

        // describe the pipeline state object
        pipelineStateDesc.pRootSignature = std::static_pointer_cast<PipelineLayoutD3D12>(desc.pipelineLayout)->rootSignature.Get();
        pipelineStateDesc.InputLayout = { inputLayout, static_cast<uint32_t>(nattributes) };
        pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pipelineStateDesc.VS = vertFunc->shaderBytecode;
        pipelineStateDesc.PS = fragFunc->shaderBytecode;
        pipelineStateDesc.DSVFormat = /*DXGI_FORMAT_D32_FLOAT*/ DXGI_FORMAT_UNKNOWN;  // use Unknown to specify that there is no depth stencil view
        pipelineStateDesc.RasterizerState = rasterizerDesc;
        pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        pipelineStateDesc.DepthStencilState = depthStencilDesc;
        pipelineStateDesc.SampleMask = UINT_MAX;
        pipelineStateDesc.SampleDesc.Count = 1;

        // create the PSO
        DX_CHECK(device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState)));
    }
}
#endif