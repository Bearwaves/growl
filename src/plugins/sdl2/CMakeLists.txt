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
	"src/sdl2_plugin.cpp"

	"src/sdl2_system.cpp"
	"src/sdl2_keyboard.cpp"
	"src/sdl2_controller.cpp"
	"src/sdl2_window.cpp"
	"src/sdl2_audio.cpp"
	)

set(HEADERS
	"src/sdl2_error.h"
	"src/sdl2_system.h"
	"src/sdl2_window.h"
	"src/sdl2_audio.h"
	)

add_library(growl-plugin-sdl2 ${SOURCES} ${HEADERS})
target_link_libraries(growl-plugin-sdl2 growl-core ${SDL_LINK_LIBS})
