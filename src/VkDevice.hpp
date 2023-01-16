#pragma once
#include "Device.hpp"
#include <optional>
#include <vulkan/vulkan.h>
#include "Pipeline.hpp"

#undef CreateSemaphore

namespace RGL {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct DeviceVk : public IDevice, public std::enable_shared_from_this<DeviceVk> {
		VkDevice device = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;	// does not need to be destroyed
		QueueFamilyIndices indices;
		VkQueue presentQueue = VK_NULL_HANDLE;	// do not need to be destroyed
		VkCommandPool commandPool = VK_NULL_HANDLE;
		virtual ~DeviceVk();
		DeviceVk(decltype(physicalDevice) physicalDevice);

		// IDevice
		std::string GetBrandString() final;
		std::shared_ptr<ISwapchain> CreateSwapchain(std::shared_ptr<ISurface>, std::shared_ptr<ICommandQueue>, int width, int height) final;
		std::shared_ptr<IRenderPass> CreateRenderPass(const RenderPassConfig&) final;
		std::shared_ptr<IPipelineLayout> CreatePipelineLayout(const PipelineLayoutDescriptor&) final;
		std::shared_ptr<IRenderPipeline> CreateRenderPipeline(const RenderPipelineDescriptor&) final;

		std::shared_ptr<IShaderLibrary> CreateShaderLibraryFromName(const std::string_view& name) final;
		std::shared_ptr<IShaderLibrary> CreateDefaultShaderLibrary() final;
		std::shared_ptr<IShaderLibrary> CreateShaderLibraryFromBytes(const std::span<uint8_t>) final;
		std::shared_ptr<IShaderLibrary> CreateShaderLibrarySourceCode(const std::string_view) final;
		std::shared_ptr<IShaderLibrary> CreateShaderLibraryFromPath(const std::filesystem::path&) final;

		std::shared_ptr<IBuffer> CreateBuffer(const BufferConfig&) final;

		std::shared_ptr<ICommandQueue> CreateCommandQueue(QueueType type) final;
		std::shared_ptr<IFence> CreateFence(bool preSignaled) final;
		std::shared_ptr<ISemaphore> CreateSemaphore() final;
		void BlockUntilIdle() final;
	};

	std::shared_ptr<IDevice> CreateDefaultDeviceVk();
}