project(growl-platform-desktop)

include_directories("include" "../../core/include" "../../plugins/dummy/include")

set(SOURCES
	"src/game_adapter.cpp"
	)

set(HEADERS
	"include/growl/platforms/desktop/game_adapter.h"
	)

add_library(growl-platform-desktop ${SOURCES} ${HEADERS})
target_link_libraries(growl-platform-desktop growl-core growl-plugin-dummy)
