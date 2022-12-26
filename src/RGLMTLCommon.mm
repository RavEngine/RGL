#include "RGLMTL.hpp"
#include "RGLCommon.hpp"

using namespace RGL;

void RGL::InitMTL() {
	Assert(CanInitAPI(RGL::API::Metal), "Metal cannot be initialized on this platform.");
	RGLGlobals::currentAPI = API::Metal;
}