cmake_minimum_required(VERSION 3.13)

project(mandel VERSION 1.0
                  DESCRIPTION "Mandelbrot set."
                  LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED MODULE)
include_directories(${SDL2_INCLUDE_DIRS})
add_executable(mandel "")

target_sources(mandel
    PRIVATE
        src/main.cpp
)


target_compile_features(mandel PUBLIC cxx_std_14)
target_link_libraries(mandel ${SDL2_LIBRARIES})

target_include_directories(mandel PUBLIC 
                           src)