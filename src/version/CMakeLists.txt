project(growl-version)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/_version.cpp PROPERTIES SYMBOLIC 1)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
		${CMAKE_CURRENT_BINARY_DIR}/_version.cpp
	COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake
	)

set(SOURCES
	${CMAKE_CURRENT_BINARY_DIR}/version.cpp
	)

set(HEADERS
	"include/growl/version.h"
	)

add_library(growl-version ${SOURCES} ${HEADERS})
target_include_directories(growl-version PUBLIC include)
set_target_properties(growl-version PROPERTIES LINKER_LANGUAGE C)
