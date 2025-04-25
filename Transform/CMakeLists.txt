cmake_minimum_required(VERSION 3.10)
project(transform)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)


add_executable(transform
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/Quaternion_Pose.cpp
)


