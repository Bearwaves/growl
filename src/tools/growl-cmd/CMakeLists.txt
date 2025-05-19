cmake_minimum_required (VERSION 3.19)
project(growl-cmd)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(SOURCES
	"src/main.cpp"
	"src/assets.cpp"
	"src/atlas.cpp"
	"src/shader_pack.cpp"
	"src/assets_config.cpp"
)

set(HEADERS
	"thirdparty/CLI11.hpp"
	"thirdparty/rang.hpp"
	"src/assets_config.h"
	"src/error.h"
)

add_executable(growl-cmd ${SOURCES} ${HEADERS})
target_link_libraries(growl-cmd
	PRIVATE
	growl-version
	growl-core
	growl-thirdparty
)
