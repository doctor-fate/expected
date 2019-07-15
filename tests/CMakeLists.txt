cmake_minimum_required(VERSION 3.1)

project(expected_tests)

set(CMAKE_CXX_STANDARD 17)

add_executable(expected_tests main.cpp expected.cpp)

target_link_libraries(expected_tests PRIVATE expected gtest)