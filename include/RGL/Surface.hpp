#pragma once
#include <memory>

namespace RGL {

	struct ISurface {

	};

	/**
	Create a surface from platform native data
	@param pointer A CAMetalLayer* (Apple), HWND* (Win32), 
	*/
	std::shared_ptr<ISurface> CreateSurfaceFromPlatformHandle(void* pointer);

}