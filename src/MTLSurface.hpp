#pragma once
#if RGL_MTL_AVAILABLE
#include <RGL/Surface.hpp>
#include <QuartzCore/CAMetalLayer.h>

namespace RGL {
	struct SurfaceMTL : public ISurface{
		CAMetalLayer* layer;
		SurfaceMTL(decltype(layer) layer ): layer(layer){}
	};

	RGLSurfacePtr CreateMTLSurfaceFromPlatformHandle(void* pointer, bool createSurfaceObject);
}

#endif

