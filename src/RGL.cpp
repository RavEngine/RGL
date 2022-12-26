#include "RGL.hpp"

using namespace RGL;

std::shared_ptr<IDevice> RGL::IDevice::CreateSystemDefaultDevice()
{
    return std::shared_ptr<IDevice>();
}

void RGL::Init(const RGLInitOptions&)
{

}
