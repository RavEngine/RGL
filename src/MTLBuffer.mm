#if RGL_MTL_AVAILABLE
#import <Metal/Metal.h>
#include "MTLBuffer.hpp"
#include "MTLDevice.hpp"
#include "RGLMTL.hpp"

namespace RGL{

BufferMTL::BufferMTL(decltype(owningDevice) owningDevice, const BufferConfig& config) : owningDevice(owningDevice){
    //TODO: make access configurable and options
    constexpr auto mode =
#if TARGET_OS_IPHONE
    MTLResourceStorageModeShared;
#else
    MTLResourceStorageModeManaged;
#endif
    
    MTL_CHECK(buffer = [owningDevice->device newBufferWithLength:config.size_bytes options: mode]);
    data.size = config.size_bytes;
    stride = config.stride;
}

void BufferMTL::MapMemory(){
    data.data = [buffer contents];
}

void BufferMTL::UnmapMemory(){
    data.data = nullptr;
#if !TARGET_OS_IPHONE
    [buffer didModifyRange:NSMakeRange(0, data.size)];
#endif
}

void BufferMTL::SetBufferData(untyped_span data, decltype(BufferConfig::size_bytes) offset){
    MapMemory();
    UpdateBufferData(data, offset);
    UnmapMemory();
}

void BufferMTL::UpdateBufferData(untyped_span newData, decltype(BufferConfig::size_bytes) offset){
    Assert(data.data != nullptr, "Must call MapMemory before updating a buffer");
    Assert(newData.size() + offset <= data.size, "Data would exceed end of buffer!");
    std::memcpy(static_cast<std::byte*>(data.data) + offset, newData.data(), newData.size());
}

decltype(BufferConfig::size_bytes) BufferMTL::getBufferSize() const{
    return data.size;
}

}
#endif
