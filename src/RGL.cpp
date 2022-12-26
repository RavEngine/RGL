#include "RGL.hpp"
#include "RGLMTL.hpp"
#include "RGLVk.hpp"
#include "RGLD3D12.hpp"
#include "RGLCommon.hpp"
#include "Device.hpp"
#include "D3D12Device.hpp"
#include "VkDevice.hpp"
#include <iostream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <format>
#endif

using namespace RGL;

STATIC(RGL::currentAPI) = API::Uninitialized;

static callback_t callbackFn = [](MessageSeverity severity, const std::string& message) {
    constexpr auto severityToStr = [](MessageSeverity severity) {
        switch (severity) {
        case MessageSeverity::Info:
            return "Info";
        case MessageSeverity::Warning:
            return "Warning";
        case MessageSeverity::Error:
            return "Error";
        case MessageSeverity::Fatal:
            return "Fatal";
        }
    };

    std::cout << "RGL [" << severityToStr(severity) << "] - " << message << "\n";
#ifdef _WIN32
    std::string str = std::format("RGL [{}] - {}\n", severityToStr(severity), message);
    OutputDebugString(str.c_str());
#endif
}; 

std::shared_ptr<IDevice> RGL::IDevice::CreateSystemDefaultDevice()
{
    switch (RGL::currentAPI) {
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
        InitD3D12(options);
#elif __APPLE__
        InitMTL(options);
#elif __linux__
        InitVk(options);
#else

#endif
    }
    else {
        switch (options.api) {
        case API::Metal:
            InitMTL(options);
            break;
        case API::Vulkan:
            InitVk(options);
            break;
        case API::Direct3D12:
            InitD3D12(options);
            break;
        case API::Noop:
            break;
        default:
            FatalError("Cannot load invalid API");
            break;
        }
    }
}

void RGL::Shutdown() {
    switch (RGL::currentAPI) {
    case API::Metal:
        FatalError("not implemented");
        break;
    case API::Direct3D12:
        FatalError("not implemented");
        break;
    case API::Vulkan:
        DeinitVk();
        break;
    }

    RGL::currentAPI = API::Uninitialized;
}

bool RGL::CanInitAPI(API api)
{
    return std::find(std::begin(APIsAvailable),std::end(APIsAvailable),api) != std::end(APIsAvailable);
}

API RGL::CurrentAPI(){
    return RGL::currentAPI;
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

void RGL::LogMessage(MessageSeverity severity, const std::string& str) {
    callbackFn(severity, str);

    // crash the program 
    // the user should not try to catch this, UB lies beyond
    if (severity == MessageSeverity::Fatal) {
        throw std::runtime_error(str);
    }
}
void RGL::FatalError(const std::string& str) {
    LogMessage(MessageSeverity::Fatal, str);
}