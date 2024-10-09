cmake_minimum_required(VERSION 3.10)

set(project_name VB-TREE)

project(${project_name} VERSION 0.0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

file(GLOB SRC_FILES src/*.c)

include_directories(src)

# Add executable target
add_executable(${project_name} ${SRC_FILES})
