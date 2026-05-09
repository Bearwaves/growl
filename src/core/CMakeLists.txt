cmake_minimum_required (VERSION 3.19)

project(growl-core)

set(SOURCES
	"src/game/game.cpp"
	"src/input/processor.cpp"
	"src/input/controller.cpp"
	"src/graphics/font_texture_atlas.cpp"
	"src/scripting/scripting_api.cpp"
	"src/network/http.cpp"
	"src/network/network_api.cpp"

	"src/assets/atlas.cpp"
	"src/assets/bitmap_font_face.cpp"
	"src/assets/bundle.cpp"
	"src/assets/distance_field_font_face.cpp"
	"src/assets/font_face.cpp"
	"src/assets/font_internal.cpp"
	"src/assets/image.cpp"
	"src/assets/local_file.cpp"

	"src/text/glyph_layout.cpp"
	"src/util/timer.cpp"
	"src/util/uuid.cpp"
	"src/interpolation.cpp"
)

set(INTERFACE_LIBS
	growl-thirdparty::json
	growl-thirdparty::fmt
	growl-thirdparty::glm
)

add_library(growl-core ${SOURCES})
target_include_directories(growl-core PUBLIC "include")

target_link_libraries(growl-core
	PUBLIC
	${INTERFACE_LIBS}
	PRIVATE
	growl-thirdparty::stb
	growl-thirdparty::freetype
	growl-thirdparty::harfbuzz
	growl-thirdparty::sodium
	growl-thirdparty::msdfgen
	growl-thirdparty::utf8cpp
	growl-thirdparty::libunibreak
	growl-thirdparty::zlib
)

if (GROWL_IMGUI)
	target_compile_definitions(growl-core PRIVATE "GROWL_IMGUI")
endif()
