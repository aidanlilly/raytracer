cmake_minimum_required(VERSION 3.10)

project(RayTracer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

add_executable(Renderer main.cpp)

target_link_libraries(Renderer PRIVATE OpenMP::OpenMP_CXX)