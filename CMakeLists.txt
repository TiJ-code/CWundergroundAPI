cmake_minimum_required(VERSION 3.15)
project(CWundergroundAPI C)

set(CMAKE_C_STANDARD 99)

find_package(CURL REQUIRED)
find_package(json-c REQUIRED)

add_library(CWundergroundAPI SHARED
        src/library.c
        include/wunderground.h
        src/wunderground.c
)

target_include_directories(CWundergroundAPI PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(CWundergroundAPI PRIVATE CURL::libcurl)
target_link_libraries(CWundergroundAPI PRIVATE json-c::json-c)

add_executable(weatherclient tests/test.c)

target_link_libraries(weatherclient PRIVATE CWundergroundAPI)
