cmake_minimum_required (VERSION 3.19)

project(growl-test-app)

set(SOURCES
	"src/test_app.cpp"
	"src/test_app_game.cpp"
	"src/input.cpp"
	)

set(HEADERS
	"src/test_app_game.h"
	"src/input.h"
	)

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /permissive-")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic")
endif ()

set(GROWL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${GROWL_PATH}/cmake/")
include(GrowlProject)

add_executable(growl-test-app ${SOURCES} ${HEADERS})
growl_project(growl-test-app)
