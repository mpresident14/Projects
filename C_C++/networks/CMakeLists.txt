# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name
project(networks)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Note: Used "EXPORT CXX=$(which clang++)" to specify clang compiler

# Set compilation flags
add_compile_options(-Wall)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")

add_executable(simple_nslookup simple_nslookup.c)
add_executable(dns dnsquery.cpp maindns.cpp)
add_executable(my_traceroute netutils.cpp dnsquery.cpp my_traceroute.cpp)
add_executable(port_scan dnsquery.cpp netutils.cpp port_scan.cpp)
