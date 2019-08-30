# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name
project(rsa)

# Set the output folder where your program will be created
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})

# Note: Used "EXPORT CXX=$(which clang++)" to specify clang compiler

# Set compilation flags
add_compile_options(-Wall)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -pthread")

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")

add_executable(rsa_encrypter.cpp rsa_decrypter.cpp rsa_utils.cpp server.cpp user.cpp main.cpp)
