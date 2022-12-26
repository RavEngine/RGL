#include "RGLMTL.hpp"
#include "RGLCommon.hpp"

using namespace RGL;

void RGL::InitMTL(const RGL::InitOptions&) {
	Assert(CanInitAPI(RGL::API::Metal), "Metal cannot be initialized on this platform.");
	RGL::currentAPI = API::Metal;
}