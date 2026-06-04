cmake_minimum_required(VERSION 4.0)
project(CWundergroundAPI C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(CWundergroundAPI SHARED src/wunderground.c)

target_include_directories(CWundergroundAPI
                           PUBLIC $(PROJECT_SOURCE_DIR)/include)
