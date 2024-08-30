cmake_minimum_required(VERSION 3.10)
project(FileLineCounter)

set(CMAKE_CXX_STANDARD 11)

# 包含头文件目录
include_directories(include)

# 源文件
add_executable(FileLineCounter
    src/main.cpp
    src/file_processor.cpp
    src/thread_pool.cpp
)

# 线程库（在Linux上链接pthread）
find_package(Threads REQUIRED)
target_link_libraries(FileLineCounter ${CMAKE_THREAD_LIBS_INIT})
