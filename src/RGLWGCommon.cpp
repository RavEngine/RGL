#if RGL_WEBGPU_AVAILABLE
#include "RGLWG.hpp"

namespace RGL{
    void InitWebGPU(const RGL::InitOptions& options){
        Assert(CanInitAPI(RGL::API::WebGPU), "WebGPU cannot be initialized on this platform.");
        RGL::currentAPI = API::WebGPU;
    }

    void DeinitWebGPU(){

    }

}

#endif