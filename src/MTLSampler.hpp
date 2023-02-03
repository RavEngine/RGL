#pragma once
#include <RGL/Sampler.hpp>
#import <Metal/Metal.h>
#include <memory>

namespace RGL{
struct DeviceMTL;

struct SamplerMTL : public ISampler{
    id<MTLSamplerState> sampler = nullptr;
    
    const std::shared_ptr<DeviceMTL> owningDevice;
    SamplerMTL(decltype(owningDevice), const SamplerConfig&);
};
}
