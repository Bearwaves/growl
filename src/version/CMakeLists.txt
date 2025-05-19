project(growl-version)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/include/growl/version.h"
)

add_library(growl-version INTERFACE)
target_include_directories(growl-version INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/include")
set_target_properties(growl-version PROPERTIES LINKER_LANGUAGE C)
