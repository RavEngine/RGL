cmake_minimum_required(VERSION 3.17)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR} CACHE INTERNAL "")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)

if(MSVC)
	add_definitions(/MP)
endif()

project(RGL)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp" "include/*.hpp" "src/*.h" "src/*.mm")
add_library(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

if(APPLE)
add_definitions("-x objective-c++")
endif()

find_package(Vulkan)
if (Vulkan_FOUND)
	set(VK_LIBS ${Vulkan_LIBRARIES})
	set(VK_INCLUDE ${Vulkan_INCLUDE_DIRS})
	target_compile_definitions(${PROJECT_NAME} PRIVATE VK_AVAILABLE=1)
else()
	target_compile_definitions(${PROJECT_NAME} PRIVATE VK_AVAILABLE=0)
endif()

if(MSVC)
	add_subdirectory(deps/DirectX-Headers EXCLUDE_FROM_ALL)
	set(DX12_LIBRARIES "d3d12.lib" "DirectX-Headers" "DirectX-Guids" "dxgi.lib" "d3dcompiler.lib")
	target_compile_definitions(${PROJECT_NAME} PRIVATE DX12_AVAILABLE=1)
else()
	target_compile_definitions(${PROJECT_NAME} PRIVATE DX12_AVAILABLE=0)
endif()

if(APPLE)
	target_compile_definitions(${PROJECT_NAME} PRIVATE MTL_AVAILABLE=1)
	set(MTL_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/deps/metal-cpp" "${CMAKE_CURRENT_LIST_DIR}/deps/metal-cpp-extensions")
	find_library(FOUNDATION_LIB Cocoa REQUIRED)
    find_library(METAL_LIB Metal REQUIRED)
    find_library(QZC_LIB QuartzCore REQUIRED)
    target_sources(${PROJECT_NAME} PRIVATE source/shaders/metal.metal)
else()
	target_compile_definitions(${PROJECT_NAME} PRIVATE MTL_AVAILABLE=0)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE 
	${VK_LIBS}
	${DX12_LIBRARIES}
	${FOUNDATION_LIB}
	${METAL_LIB}
	${QZC_LIB}
)

target_include_directories(${PROJECT_NAME} PRIVATE
	${VK_INCLUDE}
	${MTL_INCLUDE}
	"include/RGL"
)

set_target_properties("RGL" PROPERTIES 
	FOLDER "RGL"
)

set_target_properties("DirectX-Guids" PROPERTIES 
	FOLDER "RGL/Libraries"
)