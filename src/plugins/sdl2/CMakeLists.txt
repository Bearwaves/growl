project(growl-plugin-sdl2)

set(SDL2_BUILDING_LIBRARY 1)
find_package(SDL2 REQUIRED)

include_directories("../../core/include" ${SDL2_INCLUDE_DIR})

set(SDL_LINK_LIBS
	${SDL2_LIBRARY}
	)

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
	set(SDL_LINK_LIBS
		${SDL_LINK_LIBS}
		"user32" "gdi32" "winmm" "imm32" "ole32" "oleaut32" "version" "uuid" "advapi32" "setupapi" "shell32"
		)
endif ()

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
target_link_libraries(growl-plugin-sdl2 growl-core ${SDL_LINK_LIBS})
