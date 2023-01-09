#if RGL_MTL_AVAILABLE
#include "MTLShaderLibrary.hpp"
#include "MTLDevice.hpp"
namespace RGL{
ShaderLibraryMTL::ShaderLibraryMTL(decltype(owningDevice) owningDevice, const std::string_view fnstr_view) : owningDevice(owningDevice){
    NSString* fnstr = [NSString stringWithUTF8String:fnstr_view.data()];
    function = [owningDevice->defaultLibrary newFunctionWithName:fnstr];
}
}
#endif
