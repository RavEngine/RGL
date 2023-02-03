#pragma once
#include <RGL/Synchronization.hpp>

namespace RGL{

struct FenceMTL : public IFence{
    void Wait() final;
    void Reset() final;
    void Signal() final;
    virtual ~FenceMTL(){}
};

struct SemaphoreMTL : public ISemaphore{
    
};

}
