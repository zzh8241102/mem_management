cmake_minimum_required(VERSION 3.21)
project(mem_management C)

set(CMAKE_C_STANDARD 11)

add_executable(mem_management unit_tests.c memory_management.c memory_management.h)
