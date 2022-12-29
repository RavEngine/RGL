#pragma once
#include <vector>
#include <memory>
#include <optional>
#include <string>
#include "TextureFormat.hpp"
#include "ShaderLibrary.hpp"

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
			enum class Type : uint8_t {
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
		struct LayoutBindingDesc {
			uint32_t binding = 0;
			enum class Type : uint8_t {
				Sampler, CombinedImageSampler, SampledImage, StorageImage, UniformTexelBuffer, StorageTexelBuffer, UniformBuffer, StorageBuffer, UniformBufferDynamic, StorageBufferDynamic, InputAttachment
			} type;
			uint32_t descriptorCount = 0;
			enum class StageFlags {
				Vertex = 0x00000001,
				Fragment = 0x00000010,
				Compute = 0x00000020,
			} stageFlags;
			//TODO: support image samplers
		};
		std::vector<LayoutBindingDesc> bindings;
	};

	struct IPipelineLayout {

	};

	enum class PrimitiveTopology : uint8_t {
		PointList, LineList, LineStrip, TriangleList, TriangleStrip,TriangleFan,
		LineListAdjacency, LineStripAdjacency, TriangleListAdjacency, TriangleStripAdjacency,
		PatchList
	};

	struct RenderPipelineDescriptor {
		struct ShaderStageDesc {
			enum class Type : uint8_t {
				Vertex, Fragment, Compute
			} type;
			std::shared_ptr<IShaderLibrary> shaderModule;
			std::string entryPoint;
		};
		std::vector<ShaderStageDesc> stages;

		// vertex info
		struct VertexConfig {
			struct VertexBindingDesc {
				uint32_t binding, stride;
				//TODO: input rate (VkVertexInputRate)
				enum class InputRate : uint8_t {
					Vertex, Instance
				} inputRate;
			} vertexBindinDesc;

			struct VertexAttributeDesc {
				uint32_t location, binding, offset;
				enum class Format {
					Undefined,
					R32G32_SignedFloat = 103,
					R32G32B32_SignedFloat = 106

				} format;
			};
			std::vector<VertexAttributeDesc> attributeDescs;

		} vertexConfig;

		struct InputAssemblyDesc {
			PrimitiveTopology topology;
			bool primitiveRestartEnabled = false;
		} inputAssembly;

		struct ViewportDesc {		// we only support one viewport and one scissor for now
			float x = 0, y = 0,
				width = 0, height = 0,
				minDepth = 0, maxDepth = 1;  // depth values must be within [0,1] but minDepth does not need to be lower than maxDepth
		} viewport;

		struct ScissorDesc {
			std::pair<int, int> offset = { 0, 0 };
			std::pair<uint32_t, uint32_t>	extent = { 0, 0 };
		} scissor;

		struct RasterizerConfig {
			bool depthClampEnable : 1 = false;				 // if set to true, fragments out of range will be clamped instead of clipped, which we rarely want (example: shadow volumes, no need for end caps)
			bool rasterizerDiscardEnable : 1 = false;		// if true, output to the framebuffer is disabled

			enum class PolygonOverride : uint8_t {
				None, Points, Lines							// lines, points, or use the existing config
			};
			constexpr static PolygonOverride polygonOverride = PolygonOverride::None;	// not yet modifiable

			enum class CullMode : uint8_t {
				None = 0,
				Front = 0b01,
				Back = 0b10,
				Both = Front | Back
			} cullMode = CullMode::Back;

			enum class WindingOrder : uint8_t {
				Clockwise, Counterclockwise
			} windingOrder : 1 = WindingOrder::Clockwise;

			struct DepthBias {
				float clamp = 0, constantFactor = 0, slopeFactor = 0;
				bool enable = false;
			} depthBias;

		} rasterizerConfig;

		struct MultisampleConfig {
			constexpr static MSASampleCount sampleCount = MSASampleCount::C1;	// configuring multisample is currently not supported
			constexpr static bool sampleShadingEnabled = false;	
			constexpr static bool alphaToCoverageEnabled = false;
			constexpr static bool alphaToOneEnabled = false;
			
		} multisampleConfig;

		// depth stencil states (not yet supported)
		struct ColorBlendConfig {
			enum class LogicalOperation : uint8_t {
				Clear, AND, AND_Reverse, Copy, ANDInverted, Noop, XOR, OR, NOR, Equivalent, Invert, ORReverse, CopyInverted, ORInverted, NAND, Set
			} logicalOperation = LogicalOperation::Copy;
			bool logicalOpEnabled : 1 = false;
			struct ColorAttachmentConfig {

				enum class BlendFactor : uint8_t {
					Zero =0, 
					One = 1, 
					SourceColor = 2, 
					OneMinusSourceColor = 3, 
					DestColor = 4, 
					OneMinusDestColor = 5, 
					SourceAlpha = 6, 
					OneMinusSourceAlpha = 7, 
					DestAlpha = 8, 
					OneMinusDestAlpha = 9, 
					ConstantColor = 10, 
					OneMinusConstantColor = 11, 
					ConstantAlpha = 12, 
					OneMinusConstantAlpha, 
					SourceAlphaSaturate,
					Source1Color, 
					OneMinusSource1Color, 
					Source1Alpha, 
					OneMinusSource1Alpha
				} sourceColorBlendFactor = BlendFactor::One,
					destinationColorBlendFactor = BlendFactor::Zero,
					sourceAlphaBlendFactor = BlendFactor::One,
					destinationAlphaBlendFactor = BlendFactor::Zero;

				enum class BlendOperation : uint8_t {
					Add, Subtract, ReverseSubtract, Min, Max
				} colorBlendOperation = BlendOperation::Add, alphaBlendOperation = BlendOperation::Add;

				enum class ColorWriteMask : uint8_t{
					Red		= 0b0001,
					Green	= 0b0010,
					Blue	= 0b0100,
					Alpha	= 0b1000,
					RGB = Red | Green | Blue,
					RGBA = RGB | Alpha
				} colorWriteMask = ColorWriteMask::RGBA;

				bool blendEnabled : 1 = false;
			};
			std::vector<ColorAttachmentConfig> attachments;	// create one for every attachment in the pass
			float blendconstants[4] = { 0,0,0,0 };
		} colorBlendConfig;
		// list of what states are dynamic (TODO)
		std::shared_ptr<IPipelineLayout> pipelineLayout;
		std::shared_ptr<IRenderPass> renderpass;
		uint32_t subpassIndex = 0;				// what subpass in the renderpass will be used (index)
	};

	struct IRenderPipeline {

	};
}