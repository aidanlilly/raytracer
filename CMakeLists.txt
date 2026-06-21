cmake_minimum_required(VERSION 3.14)

project(RayTracer C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(glfw)

find_package(OpenMP REQUIRED)

add_executable(Renderer 
    src/main.cpp 
    src/glad.c
)

target_include_directories(Renderer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Renderer PRIVATE 
    OpenMP::OpenMP_CXX 
    glfw
)