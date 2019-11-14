cmake_minimum_required (VERSION 3.15)

project (Hello)

add_executable(Hello)
set_property(TARGET Hello PROPERTY CXX_STANDARD 20)
target_sources(Hello PRIVATE main.cpp)

find_package(OpenGL REQUIRED)
target_link_libraries(Hello opengl32)

find_package(GLEW REQUIRED)
target_link_libraries(Hello GLEW::GLEW)

find_package(glfw3 3.3.0 REQUIRED)
target_link_libraries(Hello glfw)
