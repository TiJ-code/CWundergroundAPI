cmake_minimum_required(VERSION 4.0)
project(CWundergroundAPI C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(CWundergroundAPI SHARED src/wunderground.c)

target_include_directories(CWundergroundAPI PUBLIC include)

add_executable(wu_init_test test/wunderground_init_test.c)
target_link_libraries(wu_init_test PRIVATE CWundergroundAPI)
target_include_directories(wu_init_test PRIVATE include)

enable_testing()

add_test(NAME wunderground_tests COMMAND wu_init_test)
