project(growl-plugin-sdl3)

message(STATUS "[Growl] Enabled SDL3 plugin.")

if (NOT "${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
	set(SDL_LINK_LIBS growl-thirdparty::sdl)
endif()

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
	set(SDL_LINK_LIBS
		${SDL_LINK_LIBS}
		"user32" "gdi32" "winmm" "imm32" "ole32" "oleaut32" "version" "uuid" "advapi32" "setupapi" "shell32"
		)
endif ()

set(SOURCES
	"src/sdl3_plugin.cpp"

	"src/sdl3_system.cpp"
	"src/sdl3_keyboard.cpp"
	"src/sdl3_controller.cpp"
	"src/sdl3_window.cpp"
	"src/sdl3_file.cpp"
	)

set(HEADERS
	"src/sdl3_error.h"
	"src/sdl3_system.h"
	"src/sdl3_window.h"
	"src/sdl3_file.h"
	)

add_library(growl-plugin-sdl3 ${SOURCES} ${HEADERS})
target_link_libraries(growl-plugin-sdl3
	PRIVATE
	growl-core
	${SDL_LINK_LIBS}
	)
	target_compile_definitions(growl-plugin-sdl3
		INTERFACE "GROWL_SDL3"
		PRIVATE "SDL_FUNCTION_POINTER_IS_VOID_POINTER"
		)
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
	target_compile_options(growl-plugin-sdl3 PRIVATE -sUSE_SDL=3)
endif()
