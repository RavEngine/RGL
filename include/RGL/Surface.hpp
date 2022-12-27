#pragma once
#include <memory>

namespace RGL {

	struct ISurface {

	};

	/**
	Create a surface from platform native data
	@param pointer A CAMetalLayer* (Apple), HWND* (Win32),
	@param createSurfaceObject If set to true, RGL will create the OS-specific context object, so pointer should be a NSWindow* or UIWindow*
	*/
	std::shared_ptr<ISurface> CreateSurfaceFromPlatformHandle(void* pointer, bool createSurfaceObject = false);

}
