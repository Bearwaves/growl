project(growl-plugin-sdl2)

message(STATUS "[Growl] Enabled SDL2 plugin.")

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
	set(SDL2_INCLUDE_DIR "")
else ()
	set(SDL2_BUILDING_LIBRARY 1)
	find_package(SDL2 REQUIRED)
endif()

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
	"src/sdl2_file.cpp"
	)

set(HEADERS
	"src/sdl2_error.h"
	"src/sdl2_system.h"
	"src/sdl2_window.h"
	"src/sdl2_file.h"
	)

add_library(growl-plugin-sdl2 ${SOURCES} ${HEADERS})
target_link_libraries(growl-plugin-sdl2
	PRIVATE
	growl-core
	${SDL_LINK_LIBS}
	)
target_include_directories(growl-plugin-sdl2 PUBLIC ${SDL2_INCLUDE_DIR})
target_compile_definitions(growl-plugin-sdl2 INTERFACE "GROWL_SDL2")
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
	target_compile_options(growl-plugin-sdl2 PRIVATE -sUSE_SDL=2)
	target_link_options(growl-plugin-sdl2 INTERFACE -sUSE_SDL=2)
endif()
