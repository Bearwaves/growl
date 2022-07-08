cmake_minimum_required (VERSION 3.19)

project(growl-platform-desktop)

set(SOURCES
	"src/game_adapter.cpp"
	)

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

add_library(growl-platform-desktop ${SOURCES})
target_link_libraries(growl-platform-desktop
	PRIVATE
	growl-core
	growl-plugin-sdl2
	growl-plugin-soloud
	${GROWL_DESKTOP_LIBS}
	)
target_include_directories(growl-platform-desktop PUBLIC "include")
