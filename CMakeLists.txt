cmake_minimum_required(VERSION 3.8)

if (${CMAKE_VERSION} VERSION_LESS 3.13)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.13)
endif ()

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INSTALL_GTEST OFF)
set(BUILD_GMOCK OFF)

option(ENABLE_TESTS "Generate test target" ON)

project(expected VERSION 1.0.0)

add_library(expected INTERFACE)
target_sources(expected
        INTERFACE
        ${PROJECT_SOURCE_DIR}/Public/Expected/Details/BaseCopy.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/Details/BaseCopyAssign.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/Details/BaseDestructor.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/Details/BaseExpected.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/Details/BaseExpectedStorage.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/Details/BaseMove.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/Details/BaseMoveAssign.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/Details/ExpectedStorage.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/Details/ExpectedTraits.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/Details/ExpectedUnion.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/Details/Traits.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/Details/UnexpectedTraits.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/BadExpectedAccess.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/Expected.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/Tags.hpp
        ${PROJECT_SOURCE_DIR}/Public/Expected/Unexpected.hpp)
target_include_directories(expected INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Public>)

add_executable(expected_example main.cpp)
target_link_libraries(expected_example PRIVATE expected)

if (ENABLE_TESTS)
    enable_testing()

    add_subdirectory(googletest EXCLUDE_FROM_ALL)

    add_executable(expected-test tests/expected.cpp tests/unexpected.cpp)
    target_link_libraries(expected-test PRIVATE expected gtest_main)
    add_test(NAME expected COMMAND expected-test)
endif ()