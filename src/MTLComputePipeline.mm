#if RGL_MTL_AVAILABLE
#import <Metal/Metal.h>
#include "MTLComputePipeline.hpp"
#include "MTLDevice.hpp"

namespace RGL{
    ComputePipelineMTL::ComputePipelineMTL(decltype(owningDevice) owningDevice, const RGL::ComputePipelineDescriptor &desc) : owningDevice(owningDevice) {
        
    }
    ComputePipelineMTL::~ComputePipelineMTL() {
    
    }

}

#endif
