cmake_minimum_required (VERSION 3.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
project(growl-cmd)

include_directories(../../version/include)

set(SOURCES
	"src/main.cpp"
	"src/assets.cpp"
	"src/atlas.cpp"
	"src/assets_config.cpp"
	)

set(HEADERS
	"thirdparty/CLI11.hpp"
	"thirdparty/rang.hpp"
	"src/assets_config.h"
	)

add_executable(growl-cmd ${SOURCES} ${HEADERS})
target_link_libraries(growl-cmd growl-version growl-util)
