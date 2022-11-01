cmake_minimum_required(VERSION 3.13)
project(ThreadMergeSort C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS -lpthread)

add_executable(ThreadMergeSort main.c)
