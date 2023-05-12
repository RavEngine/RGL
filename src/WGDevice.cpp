#if RGL_WEBGPU_AVAILABLE
#include "WGDevice.hpp"
#include "RGLCommon.hpp"
namespace RGL{

    RGLDevicePtr CreateDefaultDeviceWG(){
        return std::make_shared<DeviceWG>();
    }

    DeviceWG::DeviceWG(){
 
    }

std::string DeviceWG::GetBrandString() {
   return "Unknown WebGPU device";
}

RGLSwapchainPtr DeviceWG::CreateSwapchain(RGLSurfacePtr isurface, RGLCommandQueuePtr presentQueue, int width, int height){
  
}

RGLPipelineLayoutPtr DeviceWG::CreatePipelineLayout(const PipelineLayoutDescriptor& desc) {
}

RGLRenderPipelinePtr DeviceWG::CreateRenderPipeline(const RenderPipelineDescriptor& desc) {
}

RGLComputePipelinePtr DeviceWG::CreateComputePipeline(const RGL::ComputePipelineDescriptor& desc) {
}

RGLShaderLibraryPtr DeviceWG::CreateDefaultShaderLibrary() {
    FatalError("CreateDefaultShaderLibrary not implemented");
}

RGLShaderLibraryPtr DeviceWG::CreateShaderLibraryFromName(const std::string_view& name){
}

RGLShaderLibraryPtr DeviceWG::CreateShaderLibraryFromBytes(const std::span<uint8_t>) {
    FatalError("Not Implemented");
}

RGLShaderLibraryPtr DeviceWG::CreateShaderLibrarySourceCode(const std::string_view source, const FromSourceConfig& config) {
}

RGLShaderLibraryPtr DeviceWG::CreateShaderLibraryFromPath(const std::filesystem::path&) {
    FatalError("ShaderLibraryMTL");
}

RGLBufferPtr DeviceWG::CreateBuffer(const BufferConfig& config) {
}

RGLCommandQueuePtr DeviceWG::CreateCommandQueue(QueueType type) {
}

RGLFencePtr DeviceWG::CreateFence(bool preSignaled) {
}

RGLTexturePtr DeviceWG::CreateTextureWithData(const TextureConfig& config, untyped_span data){
}

RGLTexturePtr DeviceWG::CreateTexture(const TextureConfig& config){
}

RGLSamplerPtr DeviceWG::CreateSampler(const SamplerConfig& config){
}

void DeviceWG::BlockUntilIdle() {
    
}

RGL::DeviceData DeviceWG::GetDeviceData() {
    return {
       
    };
}

}

#endif