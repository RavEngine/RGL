#if RGL_MTL_AVAILABLE
#include "MTLSwapchain.hpp"

namespace RGL{

void SwapchainMTL::Resize(uint32_t width, uint32_t height){
    surface->layer.drawableSize = CGSizeMake(width, height);
    
}
void SwapchainMTL::GetNextImage(uint32_t* index, RGLSemaphorePtr semaphore) {
    auto next = [surface->layer nextDrawable];
    
    activeTextures[idx] = TextureMTL(next, {static_cast<uint32_t>([[next texture] width]), static_cast<uint32_t>([[next texture] height])});
    
    *index = this->idx;
    idx = (idx + 1) % activeTextures.size();
    
}

ITexture* SwapchainMTL::ImageAtIndex(uint32_t index) {
    return &activeTextures[index];
}

void SwapchainMTL::Present(const SwapchainPresentConfig&) {
    //[activeTextures[idx].texture present];
}

}

#endif
