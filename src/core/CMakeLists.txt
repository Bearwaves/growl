cmake_minimum_required (VERSION 3.19)

project(growl-core)

set(SOURCES
	"src/api/api.cpp"
	"src/game/game.cpp"
	"src/input/processor.cpp"
	"src/graphics/font_texture_atlas.cpp"

	"src/assets/atlas.cpp"
	"src/assets/bitmap_font_face.cpp"
	"src/assets/bundle.cpp"
	"src/assets/distance_field_font_face.cpp"
	"src/assets/font_face.cpp"
	"src/assets/font_internal.cpp"
	"src/assets/image.cpp"
	"src/assets/local_file.cpp"

	"src/text/glyph_layout.cpp"
	"src/scripting/scripting_api.cpp"
	"src/util/timer.cpp"
	"src/interpolation.cpp"
	)

set(INTERFACE_LIBS
	growl-thirdparty::json
	growl-thirdparty::fmt
	growl-thirdparty::glm
	)

if(GROWL_IMGUI)
	set(SOURCES
		${SOURCES}
		"src/imgui.cpp"
		)

	set(INTERFACE_LIBS
		${INTERFACE_LIBS}
		growl-thirdparty::imgui
		)
endif()

add_library(growl-core ${SOURCES})
target_include_directories(growl-core PUBLIC "include")

target_link_libraries(growl-core
	INTERFACE
	${INTERFACE_LIBS}
	PRIVATE
	growl-thirdparty
	)
