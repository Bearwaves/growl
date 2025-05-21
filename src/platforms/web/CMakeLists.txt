cmake_minimum_required (VERSION 3.19)

project(growl-platform-web)

message(STATUS "[Growl] Enabled Web platform.")

set(SOURCES
	"src/game_adapter.cpp"
)

add_library(growl-platform-web ${SOURCES})

set(GROWL_SDL3 ON)
set(GROWL_SDL3 ON PARENT_SCOPE)
set(GROWL_OPENGL ON)
set(GROWL_OPENGL ON PARENT_SCOPE)

target_link_libraries(growl-platform-web
	PUBLIC
	growl-core
	growl-ui
	PRIVATE
	growl-plugin-sdl3
	growl-plugin-opengl
	growl-plugin-soloud
	growl-plugin-lua
	growl-plugin-web
)
target_include_directories(growl-platform-web PUBLIC "include")
target_compile_definitions(growl-platform-web INTERFACE "GROWL_WEB")
target_link_options(growl-platform-web INTERFACE -sALLOW_MEMORY_GROWTH -sEXIT_RUNTIME -sUSE_WEBGL2=1)
