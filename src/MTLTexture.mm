#if RGL_MTL_AVAILABLE
#include "MTLTexture.hpp"
namespace RGL {

TextureMTL::TextureMTL(decltype(texture) texture, const Dimension& size) : texture(texture), ITexture(size){
    
}

Dimension TextureMTL::GetSize() const{
    return size;
}

}
#endif
