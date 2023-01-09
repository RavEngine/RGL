#pragma once
#include "Buffer.hpp"
#include <Metal/Metal.h>
#include <memory>

namespace RGL{
    struct DeviceMTL;
    struct BufferMTL : public IBuffer{
        id<MTLBuffer> buffer = nullptr;
        const std::shared_ptr<DeviceMTL> owningDevice;
        MutableSpan data;
        
        BufferMTL(decltype(owningDevice), const BufferConfig&);
        
        void MapMemory() ;
        void UnmapMemory() ;
        void UpdateBufferData(untyped_span newData) ;
        void SetBufferData(untyped_span data) ;
        virtual ~BufferMTL(){}
    };
}
