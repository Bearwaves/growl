cmake_minimum_required (VERSION 3.0)

project(growl-test-app)

set(SOURCES
	"src/test_app.cpp"
	"src/test_app_game.cpp"
	)

set(HEADERS
	"src/test_app_game.h"
	)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(GROWL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${GROWL_PATH}/cmake/")
include(GrowlProject)

add_subdirectory(${GROWL_PATH} ${CMAKE_CURRENT_BINARY_DIR}/growl)

add_executable(growl-test-app ${SOURCES} ${HEADERS})
target_link_libraries(growl-test-app ${GROWL_LIBS})
