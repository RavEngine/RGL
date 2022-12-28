#ifdef RGL_VK_AVAILABLE
#include "VkRenderPass.hpp"
#include "RGLVk.hpp"

namespace RGL{
    VkSampleCountFlagBits RGLMSA2VK(RGL::MSASampleCount samplecount) {
        switch (samplecount) {
            case decltype(samplecount)::C1: return VK_SAMPLE_COUNT_1_BIT;
            case decltype(samplecount)::C2: return VK_SAMPLE_COUNT_2_BIT;
            case decltype(samplecount)::C4: return VK_SAMPLE_COUNT_4_BIT;
            case decltype(samplecount)::C8: return VK_SAMPLE_COUNT_8_BIT;
            case decltype(samplecount)::C16: return VK_SAMPLE_COUNT_16_BIT;
            case decltype(samplecount)::C32: return VK_SAMPLE_COUNT_32_BIT;
            case decltype(samplecount)::C64: return VK_SAMPLE_COUNT_64_BIT;
        }
    }

    VkFormat RGLFormat2VK(RGL::TextureFormat format) {
        switch (format) {
        case TextureFormat::BGRA8_Unorm:
            return VK_FORMAT_B8G8R8A8_SRGB;
        default:
            FatalError("Cannot convert texture format");
        }
    }

    VkPipelineBindPoint RGLType2Vk(RenderPassConfig::SubpassDesc::Type type) {
        switch (type){
            case decltype(type)::Undefined: FatalError("Cannot use Undefined subpass type");
            case decltype(type)::Graphics: return VK_PIPELINE_BIND_POINT_GRAPHICS;
            case decltype(type)::Compute: return VK_PIPELINE_BIND_POINT_COMPUTE;
            case decltype(type)::Raytrace: return VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
        }
    }

    VkAttachmentLoadOp RGL2LoadOp(LoadAccessOperation op) {
        switch (op) {
            case decltype(op)::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
            case decltype(op)::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
            case decltype(op)::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            case decltype(op)::NotAccessed: return VK_ATTACHMENT_LOAD_OP_NONE_EXT;
        }
    }

    VkAttachmentStoreOp RGL2StoreOp(StoreAccessOperation op) {
        switch (op) {
        case decltype(op)::Store: return VK_ATTACHMENT_STORE_OP_STORE;
        case decltype(op)::None: return VK_ATTACHMENT_STORE_OP_NONE;
        case decltype(op)::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }
    }

	RenderPassVk::RenderPassVk(decltype(device) device, const RenderPassConfig& config) : device(device) {
		// translate RenderPassConfig to vulkan equivalent

        // attachments
        std::vector<VkAttachmentDescription> colorAttachments;
        {
            for (const auto& attachment : config.attachments) {
                colorAttachments.push_back(VkAttachmentDescription{
                     .format = RGLFormat2VK(attachment.format),
                        .samples = RGLMSA2VK(attachment.sampleCount),
                        .loadOp = RGL2LoadOp(attachment.loadOp),  // what to do before and after
                        .storeOp = RGL2StoreOp(attachment.storeOp),
                        .stencilLoadOp = RGL2LoadOp(attachment.stencilLoadOp),
                        .stencilStoreOp = RGL2StoreOp(attachment.stencilStoreOp),
                        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR  // see https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Render_passes (VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) for more info
                    }
                );
            }
        }

        // subpasses
        std::vector<VkSubpassDescription> subpasses;
        std::vector<std::vector<VkAttachmentReference>> inputAttachmentRefs(config.subpasses.size()), colorAttachmentRefs(config.subpasses.size());
        {
            uint32_t i = 0;
            for (const auto& subpass : config.subpasses) {
                for (auto index : subpass.inputAttachmentIndices) {
                    inputAttachmentRefs[i].push_back(VkAttachmentReference{
                            .attachment = index,
                            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                        }
                    );
                }
                for (auto index : subpass.colorAttachmentIndices) {
                    colorAttachmentRefs[i].push_back(VkAttachmentReference{
                            .attachment = index,
                            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                    });
                }

                auto& pass = subpasses.emplace_back();
                    pass.pipelineBindPoint = RGLType2Vk(subpass.type);
                    pass.colorAttachmentCount = subpass.colorAttachmentIndices.size(),
                    pass.pColorAttachments = i < subpass.colorAttachmentIndices.size() ? colorAttachmentRefs[i].data() : nullptr;
                    pass.inputAttachmentCount = subpass.inputAttachmentIndices.size();
                    pass.pInputAttachments = i < subpass.inputAttachmentIndices.size() ? inputAttachmentRefs[i].data() : nullptr;
                i++;
            }
        }

        // dependencies allow ensuring that passes execute at the right time
        VkSubpassDependency dependency{
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        };

        // full pass
        //TODO: support dependencies field of renderpassinfo
        VkRenderPassCreateInfo renderPassInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = static_cast<uint32_t>(colorAttachments.size()),
            .pAttachments = colorAttachments.data(),
            .subpassCount = static_cast<uint32_t>(subpasses.size()),
            .pSubpasses = subpasses.data(),
            .dependencyCount = 1,
            .pDependencies = &dependency
        };
        VK_CHECK(vkCreateRenderPass(device->device, &renderPassInfo, nullptr, &renderPass));
	}

	RenderPassVk::~RenderPassVk()
	{
		vkDestroyRenderPass(device->device, renderPass, nullptr);
	}

}

#endif