cmake_minimum_required (VERSION 3.19)

project(growl-core)

set(SOURCES
	"src/api/api.cpp"
	"src/game/game.cpp"
	"src/input/processor.cpp"
	"src/graphics/font_texture_atlas.cpp"
	)

add_library(growl-core ${SOURCES})
target_include_directories(growl-core PUBLIC "include")
target_link_libraries(growl-core
	PUBLIC
	growl-thirdparty::json
	growl-thirdparty::fmt
	PRIVATE
	growl-util
	growl-thirdparty
	)
