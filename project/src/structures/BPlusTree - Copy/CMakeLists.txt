cmake_minimum_required(VERSION 3.10)
project(BPlusTree)

set(CMAKE_CXX_STANDARD 17)

# Add all source files
add_executable(BPlusTree main.cpp)

# Include the directory where BPlusTree.h lives
target_include_directories(BPlusTree PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
