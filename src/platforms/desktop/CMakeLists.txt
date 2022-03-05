project(growl-platform-desktop)

include_directories(
	"include"
	"../../core/include"
	)

set(SOURCES
	"src/game_adapter.cpp"
	)

set(HEADERS
	"include/growl/platforms/desktop/game_adapter.h"
	)

if (GROWL_METAL)
	set(GROWL_DESKTOP_LIBS
		growl-plugin-metal
		)
else ()
	set(GROWL_DESKTOP_LIBS
		growl-plugin-opengl
		)
endif()

add_library(growl-platform-desktop ${SOURCES} ${HEADERS})
target_link_libraries(growl-platform-desktop
	growl-core
	growl-plugin-sdl2
	${GROWL_DESKTOP_LIBS}
	)
