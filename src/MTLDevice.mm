#if RGL_MTL_AVAILABLE

#include "MTLDevice.hpp"
#include "MTLSurface.hpp"
#include "MTLSwapchain.hpp"
#include "RGLCommon.hpp"
#include "MTLSynchronization.hpp"

namespace RGL{
std::shared_ptr<RGL::IDevice> CreateDefaultDeviceMTL(){
    auto device = MTLCreateSystemDefaultDevice();
    return std::make_shared<DeviceMTL>(device);
}

std::string DeviceMTL::GetBrandString() {
    
    auto name = [device name];
    return std::string([name UTF8String]);
}

std::shared_ptr<RGL::ISwapchain> DeviceMTL::CreateSwapchain(std::shared_ptr<ISurface> isurface, int, int){
    auto surface = std::static_pointer_cast<RGL::SurfaceMTL>(isurface);
    [surface->layer setDevice:device];
    return std::make_shared<SwapchainMTL>(surface);
}

std::shared_ptr<IRenderPass> DeviceMTL::CreateRenderPass(const RenderPassConfig&) {
    FatalError("");
}

std::shared_ptr<IPipelineLayout> DeviceMTL::CreatePipelineLayout(const PipelineLayoutDescriptor&) {
    FatalError("");
}

std::shared_ptr<IRenderPipeline> DeviceMTL::CreateRenderPipeline(const RenderPipelineDescriptor&) {
    FatalError("");
}


std::shared_ptr<IShaderLibrary> DeviceMTL::CreateDefaultShaderLibrary() {
    FatalError("");
}

std::shared_ptr<IShaderLibrary> DeviceMTL::CreateShaderLibraryFromBytes(const std::span<uint8_t>) {
    FatalError("");
}

std::shared_ptr<IShaderLibrary> DeviceMTL::CreateShaderLibrarySourceCode(const std::string_view) {
    FatalError("");
}

std::shared_ptr<IShaderLibrary> DeviceMTL::CreateShaderLibraryFromPath(const std::filesystem::path&) {
    FatalError("");
}

std::shared_ptr<IBuffer> DeviceMTL::CreateBuffer(const BufferConfig&) {
    FatalError("");
}

std::shared_ptr<ICommandQueue> DeviceMTL::CreateCommandQueue(QueueType type) {
    FatalError("");
}

std::shared_ptr<IFence> DeviceMTL::CreateFence(bool preSignaled) {
    return std::make_shared<FenceMTL>();
}

std::shared_ptr<ISemaphore> DeviceMTL::CreateSemaphore() {
    return std::make_shared<SemaphoreMTL>();
}

void DeviceMTL::BlockUntilIdle() {
    
}

}

#endif
