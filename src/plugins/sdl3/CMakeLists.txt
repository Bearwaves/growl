project(growl-plugin-sdl3)

message(STATUS "[Growl] Enabled SDL3 plugin.")

set(SDL_LINK_LIBS growl-thirdparty::sdl)

set(SOURCES
	"src/sdl3_system.cpp"
	"src/sdl3_keyboard.cpp"
	"src/sdl3_controller.cpp"
	"src/sdl3_window.cpp"
	"src/sdl3_file.cpp"
	"src/sdl3_preferences.cpp"
	"src/sdl3_haptics.cpp"
	)

set(HEADERS
	"src/sdl3_error.h"
	"src/sdl3_system.h"
	"src/sdl3_window.h"
	"src/sdl3_file.h"
	"src/sdl3_preferences.h"
	"src/sdl3_haptics.h"
	)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(SDL_LINK_LIBS
		${SDL_LINK_LIBS}
		"user32" "gdi32" "winmm" "imm32" "ole32" "oleaut32" "version" "uuid" "advapi32" "setupapi" "shell32"
		)
	set(SOURCES
		${SOURCES}
		"src/sdl3_system_windows.cpp"
		)
elseif (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	set(SOURCES
		${SOURCES}
		"src/sdl3_system_web.cpp"
		)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(SOURCES
		${SOURCES}
		"src/sdl3_system_linux.cpp"
		)
elseif (APPLE)
	set(SOURCES
		${SOURCES}
		"src/sdl3_system_macos.mm"
		)
	enable_language(OBJCXX)
else ()
	set(SOURCES
		${SOURCES}
		"src/sdl3_system_otheros.cpp"
		)
endif ()

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
