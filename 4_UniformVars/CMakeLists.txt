cmake_minimum_required (VERSION 3.15)

project (UniformVars)

add_executable(${PROJECT_NAME})

set(SOURCES main.cpp)
set(HEADERS util.h shaders.h)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${HEADERS})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} PRIVATE .)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} opengl32)

find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW)

find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)

find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2main SDL2::SDL2)
