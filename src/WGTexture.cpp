#if RGL_WEBGPU_AVAILABLE
#include "WGTexture.hpp"

namespace RGL{
TextureWG::TextureWG(decltype(drawable), const Dimension&){

}
TextureWG::TextureWG(const std::shared_ptr<DeviceWG>, const TextureConfig& config, const untyped_span){

}
TextureWG::TextureWG(const std::shared_ptr<DeviceWG>, const TextureConfig& config){

}

Dimension GetSize() const{

}

~TextureWG::~TextureWG(){
    
}

}

#endif