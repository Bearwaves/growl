cmake_minimum_required (VERSION 3.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
project(growl-cmd)

include_directories(../../version/include)

set(SOURCES
	"src/main.cpp"
	"src/assets.cpp"
	)

set(HEADERS
	"contrib/CLI11.hpp"
	"contrib/rang.hpp"
	)

add_executable(growl-cmd ${SOURCES} ${HEADERS})
target_link_libraries(growl-cmd growl-version growl-contrib)
