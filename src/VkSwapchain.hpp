#pragma once
#include "Swapchain.hpp"
#include "VkDevice.hpp"
#include "VkSurface.hpp"
#include <vector>

namespace RGL {
	struct SwapchainVK : public ISwapchain {
		std::shared_ptr<RGL::DeviceVk> owningDevice;
		std::shared_ptr<SurfaceVk> owningSurface;
		VkSwapchainKHR swapChain = VK_NULL_HANDLE;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		VkExtent2D swapChainExtent;
		VkFormat swapChainImageFormat;

		virtual ~SwapchainVK();
		SwapchainVK(decltype(owningSurface), decltype(owningDevice), int width, int height);
		void Resize(int, int) final;
	private:
		void CreateSwapChainImageViews();
		void CreateFrameBuffers();
		void DestroySwapchainIfNeeded();
	};
}