set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/freetype)
set(THIRDPARTY_SOURCES
	${THIRDPARTY_SOURCES}
	"${SOURCE_PREFIX}/src/base/ftinit.c"
	"${SOURCE_PREFIX}/src/base/ftbase.c"
	"${SOURCE_PREFIX}/src/base/ftbitmap.c"
	"${SOURCE_PREFIX}/src/base/ftglyph.c"
	"${SOURCE_PREFIX}/src/base/ftsystem.c"
	"${SOURCE_PREFIX}/src/base/ftdebug.c"
	"${SOURCE_PREFIX}/src/base/ftbbox.c"
	"${SOURCE_PREFIX}/src/sdf/sdf.c"
	"${SOURCE_PREFIX}/src/sfnt/sfnt.c"
	"${SOURCE_PREFIX}/src/smooth/smooth.c"
	"${SOURCE_PREFIX}/src/cff/cff.c"
	"${SOURCE_PREFIX}/src/pshinter/pshinter.c"
	"${SOURCE_PREFIX}/src/truetype/truetype.c"
	"${SOURCE_PREFIX}/src/raster/raster.c"
	"${SOURCE_PREFIX}/src/psnames/psnames.c"
	"${SOURCE_PREFIX}/src/psaux/psaux.c"
	"${SOURCE_PREFIX}/src/gzip/ftgzip.c"
	)

set(THIRDPARTY_INCLUDES_PUBLIC
	${THIRDPARTY_INCLUDES_PUBLIC}
	"${SOURCE_PREFIX}/include"
	)

set(THIRDPARTY_COMPILE_DEFINITIONS
	${THIRDPARTY_COMPILE_DEFINITIONS}
	FT2_BUILD_LIBRARY
	FT_CONFIG_OPTION_ERROR_STRINGS
	FT_CONFIG_OPTION_USE_PNG
	FT_CONFIG_OPTION_USE_HARFBUZZ
	FT_CONFIG_OPTION_SYSTEM_ZLIB
	)

configure_file("${CMAKE_CURRENT_LIST_DIR}/ftmodule.h.in"
	"${SOURCE_PREFIX}/include/freetype/config/ftmodule.h"
	)
