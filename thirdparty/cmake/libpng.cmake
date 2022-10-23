set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/libpng)
set(SOURCES
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

if (GROWL_IOS OR GROWL_ANDROID)
	set(SOURCES
		${SOURCES}
		"${SOURCE_PREFIX}/arm/arm_init.c"
		"${SOURCE_PREFIX}/arm/filter_neon.S"
		"${SOURCE_PREFIX}/arm/filter_neon_intrinsics.c"
		"${SOURCE_PREFIX}/arm/palette_neon_intrinsics.c"
		)
endif ()

configure_file("${SOURCE_PREFIX}/scripts/pnglibconf.h.prebuilt"
	"${CMAKE_CURRENT_BINARY_DIR}/libpng/pnglibconf.h"
	)

set(INCLUDES
	"${CMAKE_CURRENT_BINARY_DIR}/libpng"
	"${SOURCE_PREFIX}"
	)

growl_thirdparty_lib(libpng
	SOURCES ${SOURCES}
	INCLUDES PUBLIC ${INCLUDES}
	LINK growl-thirdparty::zlib
	)
