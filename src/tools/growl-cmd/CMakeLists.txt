cmake_minimum_required (VERSION 3.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
project(growl-cmd)

configure_file(version.h.in "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h")

set(SOURCES
	"src/main.cpp"
	"src/assets.cpp"
	)


set(HEADERS
	"src/version.h"
	"contrib/CLI11.hpp"
	"contrib/rang.hpp"
	)

add_executable(growl-cmd ${SOURCES} ${HEADERS})
