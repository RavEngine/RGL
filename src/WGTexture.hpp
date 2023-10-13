#pragma once
#include <RGL/Types.hpp>
#include <RGL/Texture.hpp>
#include <RGL/Span.hpp>
#include <memory>

namespace RGL{
struct DeviceWG;

struct TextureWG : public ITexture{
    
    // default constructor, don't explicity use
    TextureWG() : ITexture({0,0}){}
    virtual ~TextureWG();
    
    TextureWG(decltype(drawable), const Dimension&);
    TextureWG(const std::shared_ptr<DeviceWG>, const TextureConfig& config, const untyped_span);
    TextureWG(const std::shared_ptr<DeviceWG>, const TextureConfig& config);
    
    Dimension GetSize() const;
};

}