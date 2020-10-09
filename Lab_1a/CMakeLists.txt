cmake_minimum_required(VERSION 3.16)
project(Lab_1a)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_executable(Lab_1a main.cpp utils.h multiplier.h)