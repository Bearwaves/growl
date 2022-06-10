project(growl-util)

include_directories("include" "../../thirdparty/freetype/include")

set(SOURCES
	"src/bundle.cpp"
	"src/image.cpp"
	"src/atlas.cpp"
	"src/font_face.cpp"
	"src/bitmap_font_face.cpp"
	"src/distance_field_font_face.cpp"
	"src/font_internal.cpp"
	"src/glyph_layout.cpp"
	"src/timer.cpp"
	)

set(HEADERS
	"include/growl/util/error.h"
	"include/growl/util/timer.h"

	"include/growl/util/assets/bundle.h"
	"include/growl/util/assets/error.h"
	"include/growl/util/assets/image.h"
	"include/growl/util/assets/atlas.h"
	"include/growl/util/assets/font_face.h"

	"include/growl/util/text/glyph_layout.h"
	)

add_library(growl-util ${SOURCES} ${HEADERS})
target_link_libraries(growl-util growl-thirdparty)
