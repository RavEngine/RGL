#if RGL_WEBGPU_AVAILABLE
#include "WGShaderLibrary.hpp"
#include <librglc.hpp>

namespace RGL{
    ShaderLibraryWG::ShaderLibraryWG(decltype(owningDevice), const std::filesystem::path& pathToShader){
        WGPUShaderModuleWGSLDescriptor shaderCodeDesc;
        // Set the chained struct's header
        shaderCodeDesc.chain.next = nullptr;
        //shaderCodeDesc.chain.sType = WGPUSTypeShaderModuleWGSLDescriptor;
    }
    ShaderLibraryWG::ShaderLibraryWG(decltype(owningDevice), const std::string_view, const FromSourceConfig& config){
#ifdef RGL_CAN_RUNTIME_COMPILE  // defined in CMake
    auto result = librglc::CompileString(source, librglc::API::WebGPU, static_cast<librglc::ShaderStage>(config.stage), {
        .entrypointOutputName = "transient_fn"
    });
#endif
    }
    ShaderLibraryWG::~ShaderLibraryWG(){

    }
}

#endif