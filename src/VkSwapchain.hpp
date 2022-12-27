#pragma once
#include "Swapchain.hpp"
#include "VkDevice.hpp"
#include "VkSurface.hpp"

namespace RGL {
	struct SwapchainVK : public ISwapchain {
		std::shared_ptr<RGL::DeviceVk> owningDevice;
		std::shared_ptr<SurfaceVk> owningSurface;
		VkSwapchainKHR swapChain = VK_NULL_HANDLE;

		virtual ~SwapchainVK();
		SwapchainVK(decltype(owningSurface), decltype(owningDevice), int width, int height);
		void Resize(int, int) final;
		void DestroySwapchainIfNeeded();
	};
}