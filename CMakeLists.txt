project(ringBuffer)
cmake_minimum_required(VERSION 3.10)

add_executable(${PROJECT_NAME} main.cpp BufferQueue.cpp BufferManager.cpp)