#pragma once
#include "RGLCommon.hpp"

#define MTL_CHECK(a) {NSError* err = nullptr; a; if(err != nullptr){ NSLog(@"%@",err); assert(false);}}


namespace RGL {
	void InitMTL(const RGL::InitOptions&);
	void DeinitMTL();
}
