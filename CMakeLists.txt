cmake_minimum_required(VERSION 3.3)
project(network_pro1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES server.cpp)
add_executable(network_pro1 ${SOURCE_FILES})