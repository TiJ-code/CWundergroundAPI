cmake_minimum_required(VERSION 4.0)
project(CWundergroundAPI C)

set(CMAKE_C_STANDARD 99)

add_library(CWundergroundAPI SHARED
        src/library.c
)
