cmake_minimum_required (VERSION 3.19)

project(growl-plugin-soloud)

set(SOURCES
	"src/soloud_plugin.cpp"
	"src/soloud_audio.cpp"
	)

set(HEADERS
	"src/soloud_audio.h"
	)

add_library(growl-plugin-soloud ${SOURCES} ${HEADERS})
target_link_libraries(growl-plugin-soloud PRIVATE growl-core)
