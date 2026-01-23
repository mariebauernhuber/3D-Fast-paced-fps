cmake_minimum_required(VERSION 3.20)
project(App LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# NixOS SDL3 via pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3 sdl3-ttf)

# Fetch Catch2 for tests
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
)
FetchContent_MakeAvailable(Catch2)

# Define source files FIRST and exclude main.cpp for tests
file(GLOB SRC_FILES src/*.cpp)
list(FILTER SRC_FILES EXCLUDE REGEX "main\\.cpp$")  # Remove src/main.cpp from tests

# Common compiler flags
set(COMMON_FLAGS -Wall -Wextra -g ${SDL3_CFLAGS_OTHER})

# Common libraries
set(COMMON_LIBS ${SDL3_LIBRARIES} m GL GLEW glfw)

# ImGui sources
set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/backends/imgui_impl_sdl3.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_sdlrenderer3.cpp
)

# === TESTS ===
add_executable(unit-tests tests/unit-tests.cpp ${SRC_FILES} ${IMGUI_SOURCES})
target_link_libraries(unit-tests PRIVATE Catch2::Catch2WithMain ${COMMON_LIBS})
target_include_directories(unit-tests PRIVATE 
    include ${SDL3_INCLUDE_DIRS} 
    imgui imgui/backends
)
target_compile_options(unit-tests PRIVATE ${COMMON_FLAGS})

include(CTest)
include(Catch)
catch_discover_tests(unit-tests)

# === MAIN APP ===
set(ALL_SOURCES ${SRC_FILES} src/main.cpp ${IMGUI_SOURCES})
add_executable(app ${ALL_SOURCES})
target_link_libraries(app PRIVATE ${COMMON_LIBS})
target_include_directories(app PRIVATE 
    include ${SDL3_INCLUDE_DIRS} 
    imgui imgui/backends
)
target_compile_options(app PRIVATE ${COMMON_FLAGS})

# Output to bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Convenience targets
add_custom_target(run
    COMMAND ./bin/app
    DEPENDS app
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/*
)


