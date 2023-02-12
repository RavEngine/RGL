#pragma once
#include <RGL/Types.hpp>
#include <RGL/Buffer.hpp>
#include "MTLObjCCompatLayer.hpp"
#include <memory>

namespace RGL{
    struct DeviceMTL;
    struct BufferMTL : public IBuffer{
        OBJC_ID(MTLBuffer) buffer = nullptr;
        const std::shared_ptr<DeviceMTL> owningDevice;
        MutableSpan data;
        uint32_t stride = 0;
        
        BufferMTL(decltype(owningDevice), const BufferConfig&);
        
        //IBuffer
        void MapMemory() ;
        void UnmapMemory() ;
        void UpdateBufferData(untyped_span newData, decltype(BufferConfig::size_bytes) offset = 0) ;
        void SetBufferData(untyped_span data, decltype(BufferConfig::size_bytes) offset = 0) ;
        decltype(BufferConfig::size_bytes) getBufferSize() const final;
        virtual ~BufferMTL(){}
    };
}
