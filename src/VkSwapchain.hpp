#pragma once
#include "Swapchain.hpp"
#include "VkDevice.hpp"
#include "VkSurface.hpp"
#include "VkTexture.hpp"
#include <vector>

namespace RGL {
	struct SwapchainVK : public ISwapchain {
		std::shared_ptr<RGL::DeviceVk> owningDevice;
		std::shared_ptr<SurfaceVk> owningSurface;
		VkSwapchainKHR swapChain = VK_NULL_HANDLE;
		VkFence swapchainFence = VK_NULL_HANDLE;
		VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<TextureVk> RGLTextureResources;
		VkExtent2D swapChainExtent;
		VkFormat swapChainImageFormat;

		virtual ~SwapchainVK();
		SwapchainVK(decltype(owningSurface), decltype(owningDevice), int width, int height);
		void Resize(int, int) final;
		ITexture& GetNextImage() final;
	private:
		void CreateSwapChainImageViews();
		void DestroySwapchainIfNeeded();
	};
}