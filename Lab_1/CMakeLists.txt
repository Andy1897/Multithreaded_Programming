cmake_minimum_required(VERSION 3.16)
project(Lab_1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_executable(Lab_1 main.cpp Matrix.cpp Multiplier.cpp)