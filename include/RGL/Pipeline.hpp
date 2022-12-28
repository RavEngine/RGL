#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "TextureFormat.hpp"

namespace RGL {
	
	struct RenderPassConfig {
		struct AttachmentDesc {
			// formats, multisample count, load and store ops (see VkAttachmentDescription), and layout (from VkAttachmentReference)
			TextureFormat format;
			MSASampleCount sampleCount = MSASampleCount::C1;
			LoadAccessOperation loadOp = LoadAccessOperation::DontCare;
			StoreAccessOperation storeOp = StoreAccessOperation::DontCare;

			LoadAccessOperation stencilLoadOp = LoadAccessOperation::DontCare;
			StoreAccessOperation stencilStoreOp = StoreAccessOperation::DontCare;

			TextureLayout initialLayout;	// TODO: support (vk)
			TextureLayout finalLayout;		// TODO: support (vk)
		};
		std::vector<AttachmentDesc> attachments;

		struct SubpassDesc {	// see (VkSubpassDescription)
			// what type (graphics, compute, raytrace)
			enum class Type {
				Undefined,	// don't use this!
				Graphics,
				Compute,
				Raytrace
			} type = Type::Undefined;

			// how many Input attachments + their indices into AttachmentDescs
			std::vector<uint16_t> inputAttachmentIndices;
			// how many Color attachments + their indices into AttachmentDesc
			std::vector<uint16_t> colorAttachmentIndices;
			// DepthStencil AttachmentDescs (only have 1) (use std::optional)
			std::optional<uint16_t> depthStencilAttachmentIndex;
		};
		std::vector<SubpassDesc> subpasses;

		struct SubpassDependencyDesc {	// see VkSubpassDependency, one of these will be created implicitly for the front of the pass itself
			
		};
		std::vector<SubpassDependencyDesc> subpassDependencies;	//TODO: support (vk)

	};

	struct IRenderPass {

	};

	struct PipelineLayoutDescriptor {

	};

	struct IPipelineLayout {

	};

	struct RenderPipelineDescriptor {
		struct ShaderStageDesc {
			// vertex, fragment, compute, mesh, etc
			// shader module
			// shader entry point
		};
		// stages 
		std::vector<ShaderStageDesc> stages;

		// vertex info
		// input assembly (trilist, etc)

		// Viewport
		// Scissor
		// rasterizer states (see VkPipelineRasterizationStateCreateInfo)
		// multisample states 
		// depth stencil states
		// blend modes
		// list of what states are dynamic
		std::shared_ptr<IRenderPass> renderpass;
		// what subpass in the renderpass will be used (index)

	};

	struct IRenderPipeline {

	};
}