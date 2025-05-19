set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/zlib)
set(SOURCES
	"${SOURCE_PREFIX}/adler32.c"
	"${SOURCE_PREFIX}/compress.c"
	"${SOURCE_PREFIX}/crc32.c"
	"${SOURCE_PREFIX}/deflate.c"
	"${SOURCE_PREFIX}/infback.c"
	"${SOURCE_PREFIX}/inffast.c"
	"${SOURCE_PREFIX}/inflate.c"
	"${SOURCE_PREFIX}/inftrees.c"
	"${SOURCE_PREFIX}/trees.c"
	"${SOURCE_PREFIX}/uncompr.c"
	"${SOURCE_PREFIX}/zutil.c"
)

set(INCLUDES
	"${SOURCE_PREFIX}"
)

growl_thirdparty_lib(zlib
	SOURCES	${SOURCES}
	INCLUDES PUBLIC ${INCLUDES}
)
