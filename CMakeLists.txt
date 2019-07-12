cmake_minimum_required(VERSION 3.1)

project(expected)

add_subdirectory(googletest)

set(CMAKE_CXX_STANDARD 17)

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
	${PROJECT_SOURCE_DIR}/Public/Expected/Unexpected.hpp
)

target_include_directories(expected INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Public>)

add_executable(expected_example main.cpp)

target_link_libraries(expected_example PRIVATE expected)

add_subdirectory(test)