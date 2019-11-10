cmake_minimum_required (VERSION 3.15)

project(Tutorial VERSION 1.2)
#set_property(TARGET OUTPUT_NAME "First")

option(FOO_ENABLE "Enable Foo" ON)
if(FOO_ENABLE)
  set(FOO_STRING "i am defined in CMakeLists!")
endif()

add_executable (Tutorial "CMakeProject1.cpp" "CMakeProject1.h")
set_target_properties(Tutorial PROPERTIES OUTPUT_NAME "TutorialAsdf")
configure_file(TutorialConfig.h.in TutorialConfig.h)
target_include_directories(Tutorial PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
