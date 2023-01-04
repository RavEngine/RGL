#if RGL_VK_AVAILABLE
#include "VkRenderPipeline.hpp"
#include "RGLVk.hpp"
#include "VkShaderLibrary.hpp"
#include "VkBuffer.hpp"

namespace RGL {
    VkShaderStageFlagBits RGL2VKshader(RenderPipelineDescriptor::ShaderStageDesc::Type type) {
        switch (type) {
            case decltype(type)::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
            case decltype(type)::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
            case decltype(type)::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
            default:
                FatalError("Invalid shader type");
        }
    }

    VkPrimitiveTopology RGL2VkTopology(PrimitiveTopology top) {
        switch (top) {
            case decltype(top)::PointList: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            case decltype(top)::LineList: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case decltype(top)::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case decltype(top)::TriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case decltype(top)::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            case decltype(top)::TriangleFan: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
            case decltype(top)::LineListAdjacency: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
            case decltype(top)::LineStripAdjacency: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
            case decltype(top)::TriangleListAdjacency: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
            case decltype(top)::TriangleStripAdjacency: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
            case decltype(top)::PatchList: return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
        }
    }

    VkPolygonMode RGLK2VkPolygon(RenderPipelineDescriptor::RasterizerConfig::PolygonOverride override) {
        switch (override) {
            case decltype(override)::None: return VK_POLYGON_MODE_FILL;
            case decltype(override)::Lines: return VK_POLYGON_MODE_LINE;
            case decltype(override)::Points: return VK_POLYGON_MODE_POINT;
        }
    }

	RenderPipelineVk::RenderPipelineVk(decltype(owningDevice) device, const RenderPipelineDescriptor& desc) : owningDevice(device), renderPass(std::static_pointer_cast<RenderPassVk>(desc.renderpass)), pipelineLayout(std::static_pointer_cast<PipelineLayoutVk>(desc.pipelineLayout))
	{
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        shaderStages.reserve(desc.stages.size());

        for (const auto& stage : desc.stages) {
            shaderStages.push_back(VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = RGL2VKshader(stage.type),
                .module = std::static_pointer_cast<ShaderLibraryVk>(stage.shaderModule)->shaderModule,
                .pName = stage.entryPoint.c_str()
            });
        }

        // this allows for some minor tweaks to the pipeline object after it's created
        // at draw time, the values must be specified (required)
        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };
        VkPipelineDynamicStateCreateInfo dynamicState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data()
        };

        // setup vertex info
        VkVertexInputBindingDescription bindingDescription{
            .binding = desc.vertexConfig.vertexBindinDesc.binding,
            .stride = desc.vertexConfig.vertexBindinDesc.stride,
            .inputRate = desc.vertexConfig.vertexBindinDesc.inputRate == decltype(desc.vertexConfig.vertexBindinDesc.inputRate)::Vertex ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE
        };
        std::vector<VkVertexInputAttributeDescription>attributeDescriptions;
        attributeDescriptions.reserve(desc.vertexConfig.attributeDescs.size());
        for (const auto& attribute : desc.vertexConfig.attributeDescs) {
            attributeDescriptions.push_back(VkVertexInputAttributeDescription{
                 .location = attribute.location,
                .binding = attribute.binding,
                .format = static_cast<VkFormat>(attribute.format),   // these use the same numeric values as VkFormat for convenience
                .offset = attribute.offset
             });
        }

        // vertex format
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &bindingDescription,      // optional
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
            .pVertexAttributeDescriptions = attributeDescriptions.data(),    // optional
        };

        // trilist, tristrip, etc
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = RGL2VkTopology(desc.inputAssembly.topology),
            .primitiveRestartEnable = desc.inputAssembly.primitiveRestartEnabled      // for STRIP topology
        };

        // the viewport
        VkViewport viewport{
            .x = desc.viewport.x,
            .y = desc.viewport.height - desc.viewport.y,    // this is reversed for the same reason as the comment below
            .width = desc.viewport.width,
            .height = -desc.viewport.height,            // this is negative to convert Vulkan to use a Y-up coordinate system like the other APIs
            .minDepth = desc.viewport.minDepth,
            .maxDepth = desc.viewport.maxDepth       
        };

        // the scissor
        VkRect2D scissor{
            scissor.offset = {desc.scissor.offset.first, desc.scissor.offset.second},
            scissor.extent = {desc.scissor.extent.first, desc.scissor.extent.second}
        };
        // here's where we set the dynamic pipeline states
        VkPipelineViewportStateCreateInfo viewportState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &scissor       // arrays go here, but using multiple requires enabling a GPU feature
        };

        // fragment stage config
        VkPipelineRasterizationStateCreateInfo rasterizer{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = desc.rasterizerConfig.depthClampEnable,    // if set to true, fragments out of range will be clamped instead of clipped, which we rarely want (example: shadow volumes, no need for end caps)
            .rasterizerDiscardEnable = desc.rasterizerConfig.rasterizerDiscardEnable, // if true, output to the framebuffer is disabled
            .polygonMode = RGLK2VkPolygon(desc.rasterizerConfig.polygonOverride),        // lines, points, fill (anything other than fill requires a GPU feature)
            .cullMode = static_cast<VkCullModeFlags>(desc.rasterizerConfig.cullMode),      // front vs backface culling
            .frontFace = static_cast<VkFrontFace>(desc.rasterizerConfig.windingOrder),   // CW vs CCW 
            .depthBiasEnable = desc.rasterizerConfig.depthBias.enable,            // depth bias is useful for shadow maps
            .depthBiasConstantFactor = desc.rasterizerConfig.depthBias.constantFactor,    // the next 3 are optional
            .depthBiasClamp = desc.rasterizerConfig.depthBias.clamp,
            .depthBiasSlopeFactor = desc.rasterizerConfig.depthBias.slopeFactor,
            .lineWidth = 1.0f,                       // thickness > 1 requires the wideLines GPU feature
        };

        // a way to configure hardware anti aliasing
        // this only occurs along geometry edges
        VkPipelineMultisampleStateCreateInfo multisampling{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = RGLMSA2VK(desc.multisampleConfig.sampleCount),
            .sampleShadingEnable = desc.multisampleConfig.sampleShadingEnabled,
            .minSampleShading = 1.0f,   // the rest are optional (TODO: support these)
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = desc.multisampleConfig.alphaToCoverageEnabled,
            .alphaToOneEnable = desc.multisampleConfig.alphaToOneEnabled
        };

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
        for (const auto& attachment : desc.colorBlendConfig.attachments) {
            colorBlendAttachments.push_back(decltype(colorBlendAttachments)::value_type{
                .blendEnable = attachment.blendEnabled,
                .srcColorBlendFactor = static_cast<VkBlendFactor>(attachment.sourceColorBlendFactor), //the next 6 are optional
                .dstColorBlendFactor = static_cast<VkBlendFactor>(attachment.destinationColorBlendFactor), //optional
                .colorBlendOp = static_cast<VkBlendOp>(attachment.colorBlendOperation), // Optional
                .srcAlphaBlendFactor = static_cast<VkBlendFactor>(attachment.sourceAlphaBlendFactor),
                .dstAlphaBlendFactor = static_cast<VkBlendFactor>(attachment.destinationAlphaBlendFactor),
                .alphaBlendOp = static_cast<VkBlendOp>(attachment.alphaBlendOperation),
                .colorWriteMask = static_cast<VkColorComponentFlags>(attachment.colorWriteMask),
            });
        }
        VkPipelineColorBlendStateCreateInfo colorBlending{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = desc.colorBlendConfig.logicalOpEnabled,
            .logicOp = static_cast<VkLogicOp>(desc.colorBlendConfig.logicalOperation),    //optional
            .attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size()),           // for MRT
            .pAttachments = colorBlendAttachments.data(),  // specify all the attachments here
            .blendConstants = {desc.colorBlendConfig.blendconstants[0],desc.colorBlendConfig.blendconstants[1],desc.colorBlendConfig.blendconstants[2],desc.colorBlendConfig.blendconstants[3]},        // optional
        };

        // create the pipeline object
        VkGraphicsPipelineCreateInfo pipelineInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = static_cast<uint32_t>(shaderStages.size()),
            .pStages = shaderStages.data(),
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = pipelineLayout->layout,
            .renderPass = renderPass->renderPass,
            .subpass = desc.subpassIndex,
            .basePipelineHandle = VK_NULL_HANDLE, // optional
            .basePipelineIndex = -1 // optional
        };
        VK_CHECK(vkCreateGraphicsPipelines(owningDevice->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline));
	}
    RenderPipelineVk::~RenderPipelineVk()
    {
        vkDestroyPipeline(owningDevice->device, graphicsPipeline, nullptr);
    }

    PipelineLayoutVk::PipelineLayoutVk(decltype(owningDevice) device, const PipelineLayoutDescriptor& desc) : owningDevice(device)
    {
        std::vector<VkDescriptorSetLayoutBinding> layoutbindings;
        layoutbindings.reserve(desc.bindings.size());

        for (const auto& binding : desc.bindings) {
            layoutbindings.push_back(
                VkDescriptorSetLayoutBinding {
                  .binding = binding.binding,   // see vertex shader
                  .descriptorType = static_cast<VkDescriptorType>(binding.type),
                  .descriptorCount = binding.descriptorCount,
                  .stageFlags = static_cast<VkShaderStageFlags>(binding.stageFlags), //TODO: support stageFlags
                  .pImmutableSamplers = nullptr       // used for image samplers
                }
            );
        }
      
        VkDescriptorSetLayoutCreateInfo layoutInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(layoutbindings.size()),
            .pBindings = layoutbindings.data()
        };

        // create the descriptor set layout
        VK_CHECK(vkCreateDescriptorSetLayout(owningDevice->device, &layoutInfo, nullptr, &descriptorSetLayout));

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 1,    // the rest are optional
            .pSetLayouts = &descriptorSetLayout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
        };
        VK_CHECK(vkCreatePipelineLayout(owningDevice->device, &pipelineLayoutInfo, nullptr, &layout));

        // create descriptor pool
        VkDescriptorPoolSize poolSize{
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
        };
        VkDescriptorPoolCreateInfo poolInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = 1,       // these 1's are replaced with the maximum number of frames in flight
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
        };
        VK_CHECK(vkCreateDescriptorPool(owningDevice->device, &poolInfo, nullptr, &descriptorPool));

        // create descriptor set
        VkDescriptorSetAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,    // if multiple frames are in flight, set this to the max number of frames
        .pSetLayouts = &descriptorSetLayout, // for multiple, supply a pointer with N identical copies of descriptorSetLayout
        };
        VK_CHECK(vkAllocateDescriptorSets(owningDevice->device, &allocInfo, &descriptorSet));
    }

    PipelineLayoutVk::~PipelineLayoutVk()
    {
        vkDestroyDescriptorPool(owningDevice->device, descriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(owningDevice->device, descriptorSetLayout, nullptr);
        vkDestroyPipelineLayout(owningDevice->device, layout, nullptr);
    }

    void PipelineLayoutVk::SetLayout(const LayoutConfig& config)
    {
        VkDescriptorBufferInfo bufferInfo{
           .buffer = std::static_pointer_cast<BufferVk>(config.buffer)->buffer,
           .offset = config.offset,
           .range = config.size
        };

        VkWriteDescriptorSet descriptorWrite{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = nullptr,  // optional
            .pBufferInfo = &bufferInfo,
            .pTexelBufferView = nullptr
        };
        vkUpdateDescriptorSets(owningDevice->device, 1, &descriptorWrite, 0, nullptr);
    }
}

#endif