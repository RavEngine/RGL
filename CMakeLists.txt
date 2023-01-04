cmake_minimum_required(VERSION 3.17)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR} CACHE INTERNAL "")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)

if(MSVC)
	add_definitions(/MP)
endif()

project(RGL)

#setup main library
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp" "include/*.hpp" "src/*.h" "src/*.mm")
add_library(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# setup compiler
add_subdirectory(deps/ShaderTranspiler EXCLUDE_FROM_ALL)
file(GLOB_RECURSE LIBRGLC_SOURCES "tools/librglc/*.cpp" "tools/librglc/*.hpp")
add_library(librglc ${LIBRGLC_SOURCES}) 
target_link_libraries(librglc PRIVATE ShaderTranspiler)
target_compile_features(librglc PRIVATE cxx_std_20)
target_include_directories(librglc
	PRIVATE
		"tools/librglc/include/"
	PUBLIC
		"tools/librglc/include/"
)

# setup command line compiler utility
file(GLOB RGLC_SOURCES "tools/rglc/*.cpp" "tools/rglc/*.hpp")
add_executable(rglc ${RGLC_SOURCES})
target_compile_features(rglc PRIVATE cxx_std_20)
target_link_libraries(rglc PRIVATE librglc)
target_include_directories(rglc PRIVATE "deps/cxxopts/include")
set(rglc_path "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/rglc.exe" CACHE INTERNAL "rglc compiler path")


if(APPLE)
	add_definitions("-x objective-c++")
endif()

find_package(Vulkan)
if (Vulkan_FOUND AND NOT CMAKE_SYSTEM_NAME MATCHES "WindowsStore")
	set(VK_LIBS ${Vulkan_LIBRARIES})
	set(VK_INCLUDE ${Vulkan_INCLUDE_DIRS})
	target_compile_definitions(${PROJECT_NAME} PRIVATE RGL_VK_AVAILABLE=1)
	set(RGL_VK_AVAILABLE ON CACHE INTERNAL "RGL Vulkan")
else()
	target_compile_definitions(${PROJECT_NAME} PRIVATE RGL_VK_AVAILABLE=0)
	set(RGL_VK_AVAILABLE OFF CACHE INTERNAL "RGL Vulkan")
endif()

if(MSVC)
	add_subdirectory(deps/DirectX-Headers EXCLUDE_FROM_ALL)
	set(DX12_LIBRARIES "d3d12.lib" "DirectX-Headers" "DirectX-Guids" "dxgi.lib" "d3dcompiler.lib")
	target_compile_definitions(${PROJECT_NAME} PRIVATE RGL_DX12_AVAILABLE=1)

	set_target_properties("DirectX-Guids" PROPERTIES 
		FOLDER "RGL/Libraries"
	)
	set(RGL_DX12_AVAILABLE ON CACHE INTERNAL "RGL DX12")
else()
	target_compile_definitions(${PROJECT_NAME} PRIVATE RGL_DX12_AVAILABLE=0)
	set(RGL_DX12_AVAILABLE OFF CACHE INTERNAL "RGL DX12")
endif()

if(APPLE)
	target_compile_definitions(${PROJECT_NAME} PRIVATE RGL_MTL_AVAILABLE=1)
	set(MTL_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/deps/metal-cpp" "${CMAKE_CURRENT_LIST_DIR}/deps/metal-cpp-extensions")
	find_library(FOUNDATION_LIB Cocoa REQUIRED)
    find_library(METAL_LIB Metal REQUIRED)
    find_library(QZC_LIB QuartzCore REQUIRED)
	set(RGL_MTL_AVAILABLE ON CACHE INTERNAL "RGL Metal")
else()
	target_compile_definitions(${PROJECT_NAME} PRIVATE RGL_MTL_AVAILABLE=0)
	set(RGL_MTL_AVAILABLE OFF CACHE INTERNAL "RGL Metal")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE 
	${VK_LIBS}
	${DX12_LIBRARIES}
	${FOUNDATION_LIB}
	${METAL_LIB}
	${QZC_LIB}
)

target_include_directories(${PROJECT_NAME} 
	PRIVATE
		${VK_INCLUDE}
		${MTL_INCLUDE}
		"include/RGL"
	PUBLIC
		"include"
)

set_target_properties("RGL" "rglc" "librglc" PROPERTIES 
	FOLDER "RGL"
	XCODE_GENERATE_SCHEME ON
)

set_target_properties("SPIRV-Reflect" "ShaderTranspiler" PROPERTIES 
	FOLDER "RGL/Libraries"
)

set_target_properties("GenericCodeGen" "glslang" "MachineIndependent" "OGLCompiler" "OSDependent" "SPIRV" PROPERTIES 
	FOLDER "RGL/Libraries/glslang"
)

set_target_properties(
	"spirv-tools-build-version" 
	"spirv-tools-header-DebugInfo" 
	"spirv-tools-header-NonSemanticShaderDebugInfo100" 
	"spirv-tools-header-OpenCLDebugInfo100" 
	"spv-tools-cldi100" 
	"spv-tools-clspvreflection" 
	"spv-tools-debuginfo"
	"spv-tools-shdi100"
	"spv-tools-spv-amd-gs"
	"spv-tools-spv-amd-sb"
	"spv-tools-spv-amd-stm"
	"spv-tools-spv-amd-sevp"
	PROPERTIES 
	FOLDER "RGL/Libraries/SPIRV-Tools build"
)

set_target_properties(
	"SPIRV-Tools-opt" "SPIRV-Tools-reduce" "SPIRV-Tools-static"
	PROPERTIES 
	FOLDER "RGL/Libraries/SPIRV-Tools Libraries"
)

set_target_properties(
	"core_tables"
	"enum_string_mapping"
	"extinst_tables"
	"spirv-cross-core"
	"spirv-cross-glsl" "spirv-cross-hlsl" "spirv-cross-msl" "spirv-cross-reflect"
	PROPERTIES 
	FOLDER "RGL/Libraries/SPIRV-Cross"
)

