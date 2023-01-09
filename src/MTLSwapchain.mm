#if RGL_MTL_AVAILABLE
#include "MTLSwapchain.hpp"

namespace RGL{

void SwapchainMTL::Resize(uint32_t width, uint32_t height){
    surface->layer.drawableSize = CGSizeMake(width, height);
    
}
void SwapchainMTL::GetNextImage(uint32_t* index, std::shared_ptr<ISemaphore> semaphore) {
    
}

ITexture* SwapchainMTL::ImageAtIndex(uint32_t index) {
    
}

void SwapchainMTL::Present(const SwapchainPresentConfig&) {
    
}

}

#endif
