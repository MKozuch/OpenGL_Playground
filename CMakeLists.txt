cmake_minimum_required (VERSION 3.15)

project (OpenGL_Playground)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    message(" [INFO] VCPKG CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")
    CACHE STRING "")
endif()

add_subdirectory(Hello)
add_subdirectory(2_HelloTriangle)
add_subdirectory(3_HelloSdl)
add_subdirectory(4_UniformVars)
add_subdirectory(5_HelloGlm)