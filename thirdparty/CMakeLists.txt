project(growl-thirdparty)

add_subdirectory(glm)
add_subdirectory(json)

set(CMAKE_CXX_FLAGS "-Wno-all -msse4.1 -mpclmul")
add_compile_definitions(FT2_BUILD_LIBRARY)

include_directories("freetype/include/")

set(SOURCES
	"fmt/format.cpp"

	"fpng/fpng.cpp"

	"stb_image/stb_image.c"
	"stb_rect_pack/stb_rect_pack.c"

	"freetype/src/base/ftinit.c"
	"freetype/src/base/ftbase.c"
	"freetype/src/base/ftbitmap.c"
	"freetype/src/base/ftglyph.c"
	"freetype/src/base/ftsystem.c"
	"freetype/src/base/ftdebug.c"
	"freetype/src/base/ftbbox.c"
	"freetype/src/sdf/sdf.c"
	"freetype/src/sfnt/sfnt.c"
	"freetype/src/smooth/smooth.c"
	"freetype/src/cff/cff.c"
	"freetype/src/pshinter/pshinter.c"
	"freetype/src/truetype/truetype.c"
	"freetype/src/raster/raster.c"
	"freetype/src/psnames/psnames.c"
	"freetype/src/psaux/psaux.c"
	"freetype/src/gzip/ftgzip.c"

	"harfbuzz/src/harfbuzz.cc"

	"libunibreak/src/unibreakdef.c"
	"libunibreak/src/unibreakbase.c"
	"libunibreak/src/linebreakdef.c"
	"libunibreak/src/linebreak.c"
	"libunibreak/src/linebreakdata.c"
	)

set(HEADERS
	"fmt/core.h"
	"fmt/color.h"
	"fmt/format.h"
	"fmt/format-inl.h"

	"fpng/fpng.h"

	"stb_image/stb_image.h"
	"stb_rect_pack/stb_rect_pack.h"

	"libunibreak/src/unibreakdef.h"
	"libunibreak/src/unibreakbase.h"
	"libunibreak/src/linebreakdef.h"
	"libunibreak/src/linebreak.h"
	)

add_library(growl-thirdparty ${SOURCES} ${HEADERS})
target_link_libraries(growl-thirdparty glm::glm nlohmann_json::nlohmann_json)
