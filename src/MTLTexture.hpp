#include "Texture.hpp"
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

namespace RGL{

struct TextureMTL : public ITexture{
    id<CAMetalDrawable> texture;
    
    // default constructor, don't explicity use
    TextureMTL() : ITexture({0,0}){}
    
    TextureMTL(decltype(texture), const Dimension&);
    
    Dimension GetSize() const;
};

}
