cmake_minimum_required(VERSION 2.8)
project(bimap)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

set(SOURCE_FILES main.cpp)
add_executable(bimap ${SOURCE_FILES})