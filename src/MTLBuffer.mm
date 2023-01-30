#if RGL_MTL_AVAILABLE
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

void BufferMTL::SetBufferData(untyped_span data){
    MapMemory();
    UpdateBufferData(data);
    UnmapMemory();
}

void BufferMTL::UpdateBufferData(untyped_span newData){
    Assert(data.data != nullptr, "Must call MapMemory before updating a buffer");
    Assert(newData.size() <= data.size, "Data is too big!");
    std::memcpy(data.data, newData.data(), newData.size());
}

}
#endif
