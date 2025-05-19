set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/stb)

set(SOURCES
	"${SOURCE_PREFIX}/stb_image.c"
	"${SOURCE_PREFIX}/stb_rect_pack.c"
)

growl_thirdparty_lib(stb
	SOURCES ${SOURCES}
	INCLUDES PUBLIC "${SOURCE_PREFIX}"
)
