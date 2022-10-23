set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/freetype)
set(SOURCES
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

configure_file("${CMAKE_CURRENT_LIST_DIR}/ftmodule.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/freetype/include/freetype/config/ftmodule.h"
	)

set(DEFINITIONS
	FT2_BUILD_LIBRARY
	FT_CONFIG_OPTION_ERROR_STRINGS
	FT_CONFIG_OPTION_USE_PNG
	FT_CONFIG_OPTION_USE_HARFBUZZ
	FT_CONFIG_OPTION_SYSTEM_ZLIB
	)

growl_thirdparty_lib(freetype
	SOURCES ${SOURCES}
	DEFINITIONS ${DEFINITIONS}
	INCLUDES
		PUBLIC "${SOURCE_PREFIX}/include"
		PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/freetype/include"
	LINK
		growl-thirdparty::libpng
		growl-thirdparty::zlib
		growl-thirdparty::harfbuzz
	)
