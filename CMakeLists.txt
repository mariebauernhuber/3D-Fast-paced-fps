cmake_minimum_required(VERSION 3.10)
project(RIPWAKE VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLAD_DIR ${CMAKE_SOURCE_DIR}/deps/glad)
add_library(glad ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

find_package(glfw3 3.4 REQUIRED)
find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_executable(RIPWAKE ${SRC_FILES})

target_include_directories(RIPWAKE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(RIPWAKE glfw)
target_link_libraries(RIPWAKE OpenGL::GL)
target_link_libraries(RIPWAKE glad)
