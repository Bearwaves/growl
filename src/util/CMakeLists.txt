cmake_minimum_required (VERSION 3.19)

project(growl-util)

set(SOURCES
	"src/bundle.cpp"
	"src/image.cpp"
	"src/atlas.cpp"
	"src/font_face.cpp"
	"src/bitmap_font_face.cpp"
	"src/distance_field_font_face.cpp"
	"src/font_internal.cpp"
	"src/glyph_layout.cpp"
	"src/audio.cpp"
	"src/timer.cpp"
	)

add_library(growl-util ${SOURCES})
target_link_libraries(growl-util
	PRIVATE
	growl-thirdparty
	)
target_include_directories(growl-util
	PUBLIC
	"include"
	)
