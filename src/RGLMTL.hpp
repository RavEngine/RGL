#pragma once
#include "RGLCommon.hpp"
#include "TextureFormat.hpp"
#include "MTLObjCCompatLayer.hpp"

#define MTL_CHECK(a) {NSError* err = nullptr; a; if(err != nullptr){ NSLog(@"%@",err); assert(false);}}


namespace RGL {
	void InitMTL(const RGL::InitOptions&);
	void DeinitMTL();

    APPLE_API_TYPE(MTLPixelFormat) rgl2mtlformat(TextureFormat format);
    APPLE_API_TYPE(MTLTextureUsage) rgl2mtlTextureUsage(TextureUsage usage);
}
