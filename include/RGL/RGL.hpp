#pragma once
#include <memory>
#include <functional>
#include <string>

namespace RGL {

	enum class API : uint8_t {
		PlatformDefault,		// RGL will pick an API
		Noop,					// this API does nothing
		Metal,					// Apple Metal
		Direct3D12,				// Microsoft DirectX 12
		Vulkan					// Vulkan
	};

	constexpr static API APIsAvailable[] = {
#if RGL_MTL_AVAILABLE
		API::Metal,
#endif
#if RGL_DX12_AVAILABLE
		API::Direct3D12,
#endif
#if RGL_VK_AVAILABLE
		API::Vulkan,
#endif
		API::Noop,
	};

	bool CanInitAPI(API api);
	const char* APIToString(API);

	enum class DebugSeverity : uint8_t {
		Info = 0,
		Warning,
		Error,
		Fatal
	};

	using callback_t = std::function<void(DebugSeverity, const std::string&)>;

	struct InitOptions {
		API api = API::PlatformDefault;
		callback_t callback;
	};

	API CurrentAPI();

	struct IDevice {
		virtual ~IDevice() {}

		static std::shared_ptr<IDevice> CreateSystemDefaultDevice();
	};

	void Init(const InitOptions&);

}