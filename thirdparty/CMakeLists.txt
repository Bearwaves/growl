cmake_minimum_required (VERSION 3.19)

project(growl-thirdparty)

include(FetchContent)

add_subdirectory(glm)
add_subdirectory(utfcpp)
add_subdirectory(soloud)

FetchContent_Declare(
	json
	URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz
	)
FetchContent_MakeAvailable(json)

if (MSVC)
	set(CMAKE_CXX_FLAGS "/W0")
else ()
	set(CMAKE_CXX_FLAGS "-Wno-all -msse4.1 -mpclmul")
endif ()

find_package(OpenMP)

add_compile_definitions(
	FT2_BUILD_LIBRARY
	FT_CONFIG_OPTION_ERROR_STRINGS
	FT_CONFIG_OPTION_USE_PNG
	FT_CONFIG_OPTION_SYSTEM_ZLIB
	HAVE_FREETYPE
	)

include_directories("freetype/include/" "libpng/" "zlib/")

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

	"libpng/png.c"
	"libpng/pngerror.c"
	"libpng/pngget.c"
	"libpng/pngmem.c"
	"libpng/pngpread.c"
	"libpng/pngread.c"
	"libpng/pngrio.c"
	"libpng/pngrtran.c"
	"libpng/pngrutil.c"
	"libpng/pngset.c"
	"libpng/pngtrans.c"
	"libpng/pngwio.c"
	"libpng/pngwrite.c"
	"libpng/pngwtran.c"
	"libpng/pngwutil.c"

	"libunibreak/src/unibreakdef.c"
	"libunibreak/src/unibreakbase.c"
	"libunibreak/src/linebreakdef.c"
	"libunibreak/src/linebreak.c"
	"libunibreak/src/linebreakdata.c"

	"msdfgen/core/msdfgen.cpp"
	"msdfgen/core/Shape.cpp"
	"msdfgen/core/EdgeHolder.cpp"
	"msdfgen/core/sdf-error-estimation.cpp"
	"msdfgen/core/SignedDistance.cpp"
	"msdfgen/core/save-tiff.cpp"
	"msdfgen/core/render-sdf.cpp"
	"msdfgen/core/Contour.cpp"
	"msdfgen/core/Projection.cpp"
	"msdfgen/core/msdf-error-correction.cpp"
	"msdfgen/core/edge-coloring.cpp"
	"msdfgen/core/equation-solver.cpp"
	"msdfgen/core/shape-description.cpp"
	"msdfgen/core/MSDFErrorCorrection.cpp"
	"msdfgen/core/save-bmp.cpp"
	"msdfgen/core/contour-combiners.cpp"
	"msdfgen/core/Scanline.cpp"
	"msdfgen/core/edge-selectors.cpp"
	"msdfgen/core/Vector2.cpp"
	"msdfgen/core/edge-segments.cpp"
	"msdfgen/core/rasterization.cpp"
	"msdfgen/ext/import-font.cpp"

	"zlib/adler32.c"
	"zlib/compress.c"
	"zlib/crc32.c"
	"zlib/deflate.c"
	"zlib/infback.c"
	"zlib/inffast.c"
	"zlib/inflate.c"
	"zlib/inftrees.c"
	"zlib/trees.c"
	"zlib/uncompr.c"
	"zlib/zutil.c"
	)

set(HEADERS
	"fmt/core.h"
	"fmt/color.h"
	"fmt/format.h"
	"fmt/format-inl.h"

	"fpng/fpng.h"

	"libpng/png.h"
	"libpng/pngconf.h"
	"libpng/pngdebug.h"
	"libpng/pnginfo.h"
	"libpng/pnglibconf.h"
	"libpng/pngpriv.h"
	"libpng/pngstruct.h"

	"stb_image/stb_image.h"
	"stb_rect_pack/stb_rect_pack.h"

	"libunibreak/src/unibreakdef.h"
	"libunibreak/src/unibreakbase.h"
	"libunibreak/src/linebreakdef.h"
	"libunibreak/src/linebreak.h"

	"msdfgen/msdfgen.h"
	"msdfgen/msdfgen-ext.h"
	"msdfgen/core/save-bmp.h"
	"msdfgen/core/contour-combiners.h"
	"msdfgen/core/Bitmap.hpp"
	"msdfgen/core/Contour.h"
	"msdfgen/core/generator-config.h"
	"msdfgen/core/edge-coloring.h"
	"msdfgen/core/save-tiff.h"
	"msdfgen/core/Shape.h"
	"msdfgen/core/MSDFErrorCorrection.h"
	"msdfgen/core/ShapeDistanceFinder.h"
	"msdfgen/core/msdf-error-correction.h"
	"msdfgen/core/Scanline.h"
	"msdfgen/core/BitmapRef.hpp"
	"msdfgen/core/Bitmap.h"
	"msdfgen/core/arithmetics.hpp"
	"msdfgen/core/edge-segments.h"
	"msdfgen/core/shape-description.h"
	"msdfgen/core/sdf-error-estimation.h"
	"msdfgen/core/render-sdf.h"
	"msdfgen/core/rasterization.h"
	"msdfgen/core/equation-solver.h"
	"msdfgen/core/bitmap-interpolation.hpp"
	"msdfgen/core/Projection.h"
	"msdfgen/core/EdgeColor.h"
	"msdfgen/core/ShapeDistanceFinder.hpp"
	"msdfgen/core/edge-selectors.h"
	"msdfgen/core/EdgeHolder.h"
	"msdfgen/core/SignedDistance.h"
	"msdfgen/core/pixel-conversion.hpp"
	"msdfgen/core/Vector2.h"
	"msdfgen/ext/import-font.h"

	"zlib/crc32.h"
	"zlib/deflate.h"
	"zlib/inffast.h"
	"zlib/inffixed.h"
	"zlib/inflate.h"
	"zlib/inftrees.h"
	"zlib/trees.h"
	"zlib/zconf.h"
	"zlib/zlib.h"
	"zlib/zutil.h"
	)

add_library(growl-thirdparty ${SOURCES} ${HEADERS})
target_link_libraries(growl-thirdparty
	glm::glm
	nlohmann_json::nlohmann_json
	utf8cpp
	soloud
	)

if (OpenMP_CXX_FOUND)
	target_link_libraries(growl-thirdparty OpenMP::OpenMP_CXX)
	add_compile_definitions(MSDFGEN_USE_OPENMP)
endif ()
