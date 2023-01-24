#if RGL_MTL_AVAILABLE
#include "MTLBuffer.hpp"
#include "MTLDevice.hpp"
#include "RGLMTL.hpp"

namespace RGL{

BufferMTL::BufferMTL(decltype(owningDevice) owningDevice, const BufferConfig& config) : owningDevice(owningDevice){
    //TODO: make access configurable
    MTL_CHECK(buffer = [owningDevice->device newBufferWithLength:config.size_bytes options: MTLResourceStorageModeManaged]);
    data.size = config.size_bytes;
}

void BufferMTL::MapMemory(){
    data.data = [buffer contents];
}

void BufferMTL::UnmapMemory(){
    data.data = nullptr;
    [buffer didModifyRange:NSMakeRange(0, data.size)];
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
