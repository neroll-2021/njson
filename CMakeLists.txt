cmake_minimum_required(VERSION 3.28)

project(njson VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(njson src/main.cpp src/njson.cpp)
target_include_directories(njson PUBLIC include)