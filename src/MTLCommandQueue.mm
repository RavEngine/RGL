#if RGL_MTL_AVAILABLE
#include "MTLCommandQueue.hpp"
#include "MTLDevice.hpp"
#include "MTLCommandBuffer.hpp"

namespace RGL{

CommandQueueMTL::CommandQueueMTL(decltype(owningDevice) owningDevice) : owningDevice(owningDevice){
    commandQueue = [owningDevice->device newCommandQueue];
}

std::shared_ptr<ICommandBuffer> CommandQueueMTL::CreateCommandBuffer(){
    return std::make_shared<CommandBufferMTL>(shared_from_this());
}

void CommandQueueMTL::WaitUntilCompleted(){
    // currently does nothing
}
}

#endif
