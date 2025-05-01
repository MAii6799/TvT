cmake_minimum_required(VERSION 3.10)
project(SOLVEPNP_ARMORS)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 查找 OpenCV 包
find_package(OpenCV REQUIRED)

# 设置头文件目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # 添加头文件搜索路径
    ${OpenCV_INCLUDE_DIRS}
)

# 源文件定义
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(armors
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/Detect_Light.cpp
)

# 链接 OpenCV 库
target_link_libraries(armors ${OpenCV_LIBS})