cmake_minimum_required(VERSION 3.13)
project(ThreadMergeSort C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-fopenmp -lpthread -O2")

add_executable(ThreadMergeSort main.c)
