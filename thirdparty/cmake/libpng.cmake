set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/libpng)
set(THIRDPARTY_SOURCES
	${THIRDPARTY_SOURCES}
	"${SOURCE_PREFIX}/png.c"
	"${SOURCE_PREFIX}/pngerror.c"
	"${SOURCE_PREFIX}/pngget.c"
	"${SOURCE_PREFIX}/pngmem.c"
	"${SOURCE_PREFIX}/pngpread.c"
	"${SOURCE_PREFIX}/pngread.c"
	"${SOURCE_PREFIX}/pngrio.c"
	"${SOURCE_PREFIX}/pngrtran.c"
	"${SOURCE_PREFIX}/pngrutil.c"
	"${SOURCE_PREFIX}/pngset.c"
	"${SOURCE_PREFIX}/pngtrans.c"
	"${SOURCE_PREFIX}/pngwio.c"
	"${SOURCE_PREFIX}/pngwrite.c"
	"${SOURCE_PREFIX}/pngwtran.c"
	"${SOURCE_PREFIX}/pngwutil.c"
	)

configure_file("${SOURCE_PREFIX}/scripts/pnglibconf.h.prebuilt"
	"${CMAKE_CURRENT_BINARY_DIR}/libpng/pnglibconf.h"
	)

set(THIRDPARTY_INCLUDES_PRIVATE
	${THIRDPARTY_INCLUDES_PRIVATE}
	"${CMAKE_CURRENT_BINARY_DIR}/libpng"
	"${SOURCE_PREFIX}/libpng"
	)
