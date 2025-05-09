cmake_minimum_required (VERSION 3.19)

project(growl-plugin-soloud)

set(SOURCES
	"src/soloud_audio.cpp"
	"src/bundle_file.cpp"
	"src/soloud_clip.cpp"
	"src/soloud_stream.cpp"
	)

set(HEADERS
	"src/soloud_audio.h"
	"src/bundle_file.h"
	"src/soloud_error.h"
	)

add_library(growl-plugin-soloud ${SOURCES} ${HEADERS})
target_link_libraries(growl-plugin-soloud
	PRIVATE growl-core growl-thirdparty::soloud
	)
