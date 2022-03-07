project(growl-version)

include_directories(include)

configure_file(version.h.in "${CMAKE_CURRENT_SOURCE_DIR}/include/growl/version.h")

set(HEADERS
	"include/growl/version.h"
	)

add_library(growl-version ${HEADERS})
set_target_properties(growl-version PROPERTIES LINKER_LANGUAGE CXX)
