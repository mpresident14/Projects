# Create Makefile with "CC=$(which clang) CXX=$(which clang++) cmake . "
# NOTE: Already added CC and CXX as env vars, so no longer necessary
# Then build executables with "make -j <threads>"

# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name
project(regex)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Set compilation flags
add_compile_options(-Wall -g -O0)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/../parsers")

add_executable(main main.cpp regex.cpp)
