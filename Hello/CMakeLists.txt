cmake_minimum_required (VERSION 3.15)

project ("Hello")

add_executable(Hello)
target_sources(Hello PRIVATE main.cpp)
set_property(TARGET Hello PROPERTY CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
target_link_libraries(Hello opengl32)

find_package(GLEW REQUIRED)
target_link_libraries(Hello GLEW::GLEW)

find_package(glfw3 REQUIRED)
target_link_libraries(Hello glfw)
