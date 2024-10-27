cmake_minimum_required(VERSION 3.10)
set(project_name VB-TREE)
project(${project_name} VERSION 0.0.1)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add debug flags
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -O0")

# Get source files
file(GLOB SRC_FILES src/*.c)

# Include directories
include_directories(src)

# Add executable target
add_executable(${project_name} ${SRC_FILES})

# Print build type info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C flags debug: ${CMAKE_C_FLAGS_DEBUG}")
