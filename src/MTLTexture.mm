#if RGL_MTL_AVAILABLE
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include "MTLTexture.hpp"
#include "MTLDevice.hpp"


namespace RGL {

TextureMTL::TextureMTL(decltype(drawable) texture, const Dimension& size) : drawable(texture), ITexture(size){
    
}

Dimension TextureMTL::GetSize() const{
    return size;
}

TextureMTL::TextureMTL(const std::shared_ptr<DeviceMTL> owningDevice, const TextureConfig& config, const untyped_span data) : ITexture({config.width, config.height}){
    
    //TODO: convert format
    MTLPixelFormat format = MTLPixelFormatRGBA8Uint;
    
    auto desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:format width:config.width height:config.height mipmapped:config.mipLevels > 1];
    desc.usage = MTLTextureUsageShaderRead;
    texture = [owningDevice->device newTextureWithDescriptor:desc];
    
    MTLRegion region = {
        { 0, 0, 0 },                   // MTLOrigin
        {config.width, config.height, 1} // MTLSize
    };
    
    NSUInteger bytesPerRow = 4 * config.width;   // TODO: replace 4 with nchannels of format
    
    [texture replaceRegion:region
                mipmapLevel:0
                  withBytes:data.data()
                bytesPerRow:bytesPerRow];
}

}
#endif
