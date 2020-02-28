# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name
project(rsa2)

# Set the output folder where your program will be created
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})

# Set compilation flags
add_compile_options(-Wall)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")

# Create executable
add_executable(main rsa_utils.cpp keygen.cpp server.cpp user.cpp main.cpp)

# Specify other library to link
target_link_libraries(main boost_system pthread)
