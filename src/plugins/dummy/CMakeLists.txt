project(growl-plugin-dummy)

include_directories("include" "../../core/include")

set(SOURCES
	"src/dummy_system.cpp"
	"src/dummy_graphics.cpp"
	)

set(HEADERS
	"include/growl/plugins/dummy/dummy_system.h"
	"include/growl/plugins/dummy/dummy_graphics.h"
	)

add_library(growl-plugin-dummy ${SOURCES} ${HEADERS})
target_link_libraries(growl-plugin-dummy growl-core)
