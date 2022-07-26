set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})
set(THIRDPARTY_SOURCES
	${THIRDPARTY_SOURCES}
	"${SOURCE_PREFIX}/libunibreak/src/unibreakdef.c"
	"${SOURCE_PREFIX}/libunibreak/src/unibreakbase.c"
	"${SOURCE_PREFIX}/libunibreak/src/linebreakdef.c"
	"${SOURCE_PREFIX}/libunibreak/src/linebreak.c"
	"${SOURCE_PREFIX}/libunibreak/src/linebreakdata.c"
	)

set(THIRDPARTY_INCLUDES
	${THIRDPARTY_INCLUDES}
	"${SOURCE_PREFIX}/libunibreak/src"
	)
