cmake_minimum_required (VERSION 3.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
project(growl-cmd)

set(SOURCES
	"src/main.cpp"
	)


set(HEADERS
	"contrib/CLI11.hpp"
	)

add_executable(growl-cmd ${SOURCES} ${HEADERS})
