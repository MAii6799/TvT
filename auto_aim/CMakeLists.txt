cmake_minimum_required(VERSION 3.10)
project(AUTO_AIM)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 查找 OpenCV 包
find_package(OpenCV REQUIRED)

# 查找 Eigen3 包
find_package(Eigen3 REQUIRED)

# 设置头文件目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # 添加头文件搜索路径
    ${OpenCV_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}  # 添加 Eigen3 头文件路径
)

# 源文件定义
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(autoaim
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/armor_detector.cpp
    ${SOURCE_DIR}/video_processor.cpp
    ${SOURCE_DIR}/tracker.cpp
)

# 链接 OpenCV 库
target_link_libraries(autoaim ${OpenCV_LIBS})