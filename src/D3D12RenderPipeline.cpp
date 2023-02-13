#if RGL_DX12_AVAILABLE
#include "D3D12RenderPipeline.hpp"
#include "D3D12Device.hpp"
#include "D3D12ShaderLibrary.hpp"
#include "D3D12Sampler.hpp"

namespace RGL {
    DXGI_FORMAT rgl2dxgiformat(RenderPipelineDescriptor::VertexConfig::VertexAttributeDesc::Format format) {
        switch (format) {
        case decltype(format)::R32G32B32_SignedFloat:   return DXGI_FORMAT_R32G32B32_FLOAT;
        case decltype(format)::R32G32_SignedFloat:      return DXGI_FORMAT_R32G32_FLOAT;
        default:
            FatalError("Unsupported vertex attribute format");
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

    D3D12_COMPARISON_FUNC rgl2d3dcompfn(decltype(RenderPipelineDescriptor::DepthStencilConfig::depthFunction) depthFunction) {
        switch (depthFunction) {
        case decltype(depthFunction)::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
        case decltype(depthFunction)::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
        case decltype(depthFunction)::Greater: return D3D12_COMPARISON_FUNC_GREATER;
        case decltype(depthFunction)::GreaterOrEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        case decltype(depthFunction)::Less: return D3D12_COMPARISON_FUNC_LESS;
        case decltype(depthFunction)::LessOrEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case decltype(depthFunction)::Never: return D3D12_COMPARISON_FUNC_NEVER;
        case decltype(depthFunction)::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        default:
            FatalError("Compare function not supported");

        }
    }


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
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        // create the constants data
        const auto nconstants = desc.constants.size();

        uint32_t nsamplers = 0;
        uint32_t nbuffers = 0;
        for (const auto& item : desc.bindings) {
            switch (item.type) {
            case decltype(item.type)::CombinedImageSampler:
                nsamplers++;
                break;
            case decltype(item.type)::UniformBuffer:
                nbuffers++;
                break;
            }
        }

        const auto totalParams = nconstants + (nsamplers * 2) + nbuffers;
        stackarray(rootParameters, CD3DX12_ROOT_PARAMETER1, totalParams);
        for (int i = 0; i < nconstants; i++) {
            rootParameters[i].InitAsConstants(desc.constants[i].size_bytes / sizeof(int), desc.constants[i].n_register, 0, D3D12_SHADER_VISIBILITY_ALL);
        }
        //TODO: check 
        for (int i = 0; i < nsamplers * 2; i+=2) {
            // sampler
            {
                D3D12_DESCRIPTOR_RANGE1 range{
                    .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
                    .NumDescriptors = 1,
                    .BaseShaderRegister = 0,
                    .RegisterSpace = 0,
                    .OffsetInDescriptorsFromTableStart = 0,
                };
                rootParameters[i + nconstants].InitAsDescriptorTable(1, &range);
            }

            // SRV
            {
                D3D12_DESCRIPTOR_RANGE1 range{
                    .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
                    .NumDescriptors = 1,
                    .BaseShaderRegister = 0,
                    .RegisterSpace = 0,
                    .OffsetInDescriptorsFromTableStart = 0,
                };
                rootParameters[i + 1 + nconstants].InitAsDescriptorTable(1, &range);
            }
        }
        // constant / uniform buffer bindings (SRVs)
        uint32_t buffidx = 0;
        for (const auto& item : desc.bindings) {
            if (item.type == decltype(item.type)::UniformBuffer) {
                rootParameters[buffidx + (nsamplers * 2) + nconstants].InitAsShaderResourceView(item.binding, 0);
            }
        }

        stackarray(samplerStates, D3D12_STATIC_SAMPLER_DESC, nsamplers);
        {
            uint32_t i = 0;
            for (const auto& isampler : desc.boundSamplers) {
                auto sampler = std::static_pointer_cast<SamplerD3D12>(isampler);
                auto& samplerDesc = sampler->samplerDesc;
                samplerStates[i] = {
                    .Filter = samplerDesc.Filter,
                    .AddressU = samplerDesc.AddressU,
                    .AddressV = samplerDesc.AddressV,
                    .AddressW = samplerDesc.AddressW,
                    .MipLODBias = samplerDesc.MipLODBias,
                    .MaxAnisotropy = samplerDesc.MaxAnisotropy,
                    .ComparisonFunc = samplerDesc.ComparisonFunc,
                    .BorderColor = {},
                    .MinLOD = samplerDesc.MinLOD,
                    .MaxLOD = samplerDesc.MaxLOD,
                    .ShaderRegister = i,
                    .RegisterSpace = 0,             //TODO: set register
                    .ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
                };
            }
        }


        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
        rootSignatureDescription.Init_1_1(totalParams, rootParameters, 0, nullptr, rootSignatureFlags);

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
        boundSamplers = config.boundTextures;
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
        depthStencilDesc.DepthEnable = desc.depthStencilConfig.depthTestEnabled;
        depthStencilDesc.DepthFunc = rgl2d3dcompfn(desc.depthStencilConfig.depthFunction);
        depthStencilDesc.DepthWriteMask = desc.depthStencilConfig.depthWriteEnabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;

        //TODO: complete stencil
        depthStencilDesc.StencilEnable = desc.depthStencilConfig.stencilTestEnabled;

        // describe the pipeline state object
        pipelineStateDesc.pRootSignature = std::static_pointer_cast<PipelineLayoutD3D12>(desc.pipelineLayout)->rootSignature.Get();
        pipelineStateDesc.InputLayout = { inputLayout, static_cast<uint32_t>(nattributes) };
        pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pipelineStateDesc.VS = vertFunc->shaderBytecode;
        pipelineStateDesc.PS = fragFunc->shaderBytecode;
        pipelineStateDesc.DSVFormat = rgl2dxgiformat_texture(desc.depthStencilConfig.depthFormat);
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