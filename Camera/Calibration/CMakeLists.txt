cmake_minimum_required(VERSION 3.10)
project(CALIBRATION)

# 查找 OpenCV 包
find_package(OpenCV REQUIRED)

# 添加可执行文件
add_executable(Calibration Sorryyy.cpp)

# 链接 OpenCV 库
target_link_libraries(Calibration ${OpenCV_LIBS})

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)