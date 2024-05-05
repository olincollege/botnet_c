cmake_minimum_required(VERSION 3.10)
project(Util)

# Add util library
add_library(util SHARED util.c util.h)

# Include headers
target_include_directories(util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
