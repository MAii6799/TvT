cmake_minimum_required(VERSION 3.10)
project(Homework2)  # 给你的项目起个名字

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Eigen3库
find_package(Eigen3 REQUIRED)

# 包含头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIRS}
)

# 添加可执行文件
add_executable(homework2
    src/main.cpp
    src/KalmanFilter.cpp
)

# 链接Eigen3
target_link_libraries(homework2 Eigen3::Eigen)