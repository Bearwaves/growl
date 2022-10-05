cmake_minimum_required (VERSION 3.19)

project(growl-platform-desktop)

message(STATUS "[Growl] Enabled Desktop platform.")

set(SOURCES
	"src/game_adapter.cpp"
	)

add_library(growl-platform-desktop ${SOURCES})

set(GROWL_SDL2 ON)
set(GROWL_SDL2 ON PARENT_SCOPE)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND NOT GROWL_OPENGL)
	set(GROWL_METAL ON)
	set(GROWL_METAL ON PARENT_SCOPE)
else ()
	set(GROWL_OPENGL ON)
	set(GROWL_OPENGL ON PARENT_SCOPE)
endif()

if (GROWL_METAL)
	set(GROWL_DESKTOP_LIBS
		growl-plugin-metal
		)
endif()

if (GROWL_OPENGL)
	set(GROWL_DESKTOP_LIBS
		${GROWL_DESKTOP_LIBS}
		growl-plugin-opengl
		)
endif()

target_link_libraries(growl-platform-desktop
	PUBLIC
	growl-core
	PRIVATE
	growl-plugin-sdl2
	growl-plugin-soloud
	${GROWL_DESKTOP_LIBS}
	)
target_include_directories(growl-platform-desktop PUBLIC "include")
target_compile_definitions(growl-platform-desktop INTERFACE "GROWL_DESKTOP")
