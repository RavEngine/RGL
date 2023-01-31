#pragma once
#include "Texture.hpp"
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include "Span.hpp"
#include <memory>

namespace RGL{
struct DeviceMTL;

struct TextureMTL : public ITexture{
    id<CAMetalDrawable> drawable;
    id<MTLTexture> texture;
    
    // default constructor, don't explicity use
    TextureMTL() : ITexture({0,0}){}
    
    TextureMTL(decltype(drawable), const Dimension&);
    TextureMTL(const std::shared_ptr<DeviceMTL>, const TextureConfig& config, const untyped_span);
    
    Dimension GetSize() const;
};

}
