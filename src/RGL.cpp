#include "RGL.hpp"
#include "RGLMTL.hpp"
#include "RGLVk.hpp"
#include "RGLD3D12.hpp"
#include "RGLCommon.hpp"
#include "Device.hpp"
#include "D3D12Device.hpp"
#include "VkDevice.hpp"
#include <iostream>

using namespace RGL;

API RGL::RGLGlobals::currentAPI = API::Uninitialized;

static callback_t callbackFn = [](DebugSeverity severity, const std::string& message) {
    constexpr auto severityToStr = [](DebugSeverity severity) {
        switch (severity) {
        case DebugSeverity::Info:
            return "Info";
        case DebugSeverity::Warning:
            return "Warning";
        case DebugSeverity::Error:
            return "Error";
        case DebugSeverity::Fatal:
            return "Fatal";
        }
    };

    std::cout << "RGL [" << severityToStr(severity) << "] - " << message << "\n";
}; 

std::shared_ptr<IDevice> RGL::IDevice::CreateSystemDefaultDevice()
{
    switch (RGL::RGLGlobals::currentAPI) {
    case API::Uninitialized:
        FatalError("RGL is not initialized! Call RGL::Init before using any RGL functions.");
        break;
    case API::Direct3D12:
        return CreateDefaultDeviceD3D12();
    case API::Vulkan:
        return CreateDefaultDeviceVk();
    }
    return std::shared_ptr<IDevice>();
}


void RGL::Init(const InitOptions& options)
{
    if (options.callback) {
        callbackFn = options.callback;
    }

    if (options.api == API::PlatformDefault) {
        // need to figure out what API to use
#ifdef _WIN32
        InitD3D12();
#elif __APPLE__
        InitMTL();
#elif __linux__
        InitVk();
#else

#endif
    }
    else {
        switch (options.api) {
        case API::Metal:
            InitMTL();
            break;
        case API::Vulkan:
            InitVk();
            break;
        case API::Direct3D12:
            InitD3D12();
            break;
        case API::Noop:
            break;
        default:
            FatalError("Cannot load invalid API");
            break;
        }
    }
}

bool RGL::CanInitAPI(API api)
{
    return std::find(std::begin(APIsAvailable),std::end(APIsAvailable),api) != std::end(APIsAvailable);
}

API RGL::CurrentAPI(){
    return RGLGlobals::currentAPI;
}

const char* RGL::APIToString(API api) {
    switch (api) {
    case API::Metal:
        return "Metal";
    case API::Direct3D12:
        return "Direct3D12";
    case API::Vulkan:
        return "Vulkan";
    case API::Noop:
        return "Noop";
    default:
        FatalError("APIToString: invalid API");
        return "";
    }
}

void RGL::LogMessage(DebugSeverity severity, const std::string& str) {
    callbackFn(severity, str);

    // crash the program 
    // the user should not try to catch this, UB lies beyond
    if (severity == DebugSeverity::Fatal) {
        throw std::runtime_error(str);
    }
}
void RGL::FatalError(const std::string& str) {
    LogMessage(DebugSeverity::Fatal, str);
}