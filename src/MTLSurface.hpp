#pragma once
#if RGL_MTL_AVAILABLE
#include "Surface.hpp"
#include <QuartzCore/CAMetalLayer.h>

namespace RGL {
	struct SurfaceMTL : public ISurface{
		CAMetalLayer* layer;
		SurfaceMTL(decltype(layer) layer ): layer(layer){}
	};

	std::shared_ptr<ISurface> CreateMTLSurfaceFromPlatformHandle(void* pointer, bool createSurfaceObject);
}

#endif

