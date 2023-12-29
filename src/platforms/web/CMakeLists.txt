cmake_minimum_required (VERSION 3.19)

project(growl-platform-web)

message(STATUS "[Growl] Enabled Web platform.")

set(SOURCES
	"src/game_adapter.cpp"
	)

add_library(growl-platform-web ${SOURCES})

set(GROWL_SDL2 ON)
set(GROWL_SDL2 ON PARENT_SCOPE)
set(GROWL_OPENGL ON)
set(GROWL_OPENGL ON PARENT_SCOPE)

target_link_libraries(growl-platform-web
	PUBLIC
	growl-core
	growl-scene
	PRIVATE
	growl-plugin-sdl2
	growl-plugin-opengl
	growl-plugin-soloud
	growl-plugin-lua
	)
target_include_directories(growl-platform-web PUBLIC "include")
target_compile_definitions(growl-platform-web INTERFACE "GROWL_WEB")
target_link_options(growl-platform-web INTERFACE -sALLOW_MEMORY_GROWTH -sEXIT_RUNTIME -sUSE_SDL=2 -sUSE_WEBGL2=1)
