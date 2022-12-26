#pragma once
#include <memory>

namespace RGL {

	enum class API {
		PlatformDefault,
		Noop,
		Metal,
		DirectX,
		Vulkan
	};

	struct RGLInitOptions {
		API api = API::PlatformDefault;
	};
	

	struct IDevice {
		virtual ~IDevice() {}

		static std::shared_ptr<IDevice> CreateSystemDefaultDevice();
	};

	static void Init(const RGLInitOptions&);

}