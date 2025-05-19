set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/date)

set(INCLUDES
	"${SOURCE_PREFIX}/include"
)

growl_thirdparty_lib(date
	INCLUDES INTERFACE ${INCLUDES}
)
