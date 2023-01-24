#pragma once
#include "CommandQueue.hpp"
#import <Metal/Metal.h>
#include <memory>

namespace RGL{
struct DeviceMTL;

struct CommandQueueMTL : public ICommandQueue, public std::enable_shared_from_this<CommandQueueMTL>{
    const std::shared_ptr<DeviceMTL> owningDevice;
    id<MTLCommandQueue> commandQueue = nullptr;
    CommandQueueMTL(decltype(owningDevice));
    
    std::shared_ptr<ICommandBuffer> CreateCommandBuffer() final;

    void WaitUntilCompleted() final;
    virtual ~CommandQueueMTL(){}
};
}
