set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/libunibreak)
set(THIRDPARTY_SOURCES
	${THIRDPARTY_SOURCES}
	"${SOURCE_PREFIX}/src/unibreakdef.c"
	"${SOURCE_PREFIX}/src/unibreakbase.c"
	"${SOURCE_PREFIX}/src/linebreakdef.c"
	"${SOURCE_PREFIX}/src/linebreak.c"
	"${SOURCE_PREFIX}/src/linebreakdata.c"
	)

set(THIRDPARTY_INCLUDES_PUBLIC
	${THIRDPARTY_INCLUDES_PUBLIC}
	"${SOURCE_PREFIX}/src"
	)
