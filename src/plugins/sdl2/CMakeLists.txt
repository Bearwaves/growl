project(growl-plugin-sdl2)

set(SDL2_BUILDING_LIBRARY 1)
find_package(SDL2 REQUIRED)

include_directories("../../core/include" ${SDL2_INCLUDE_DIR})

set(SOURCES
	"src/sdl_plugin.cpp"

	"src/sdl_system.cpp"
	"src/sdl_keyboard.cpp"
	"src/sdl_controller.cpp"
	"src/sdl_window.cpp"
	)

set(HEADERS
	"src/sdl_error.h"
	"src/sdl_system.h"
	"src/sdl_window.h"
	)

add_library(growl-plugin-sdl2 ${SOURCES} ${HEADERS})
target_link_libraries(growl-plugin-sdl2 growl-core ${SDL2_LIBRARY})
