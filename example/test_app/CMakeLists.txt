cmake_minimum_required (VERSION 3.19)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(growl-test-app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
add_subdirectory(${GROWL_PATH} ${CMAKE_CURRENT_BINARY_DIR}/growl)

add_executable(growl-test-app ${SOURCES} ${HEADERS})
target_link_libraries(growl-test-app PRIVATE growl-platform-desktop)
