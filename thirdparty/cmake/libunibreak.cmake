set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/libunibreak)
set(SOURCES
	"${SOURCE_PREFIX}/src/unibreakdef.c"
	"${SOURCE_PREFIX}/src/unibreakbase.c"
	"${SOURCE_PREFIX}/src/linebreakdef.c"
	"${SOURCE_PREFIX}/src/linebreak.c"
	"${SOURCE_PREFIX}/src/linebreakdata.c"
)

growl_thirdparty_lib(libunibreak
	SOURCES ${SOURCES}
	INCLUDES PUBLIC "${SOURCE_PREFIX}/src"
)
