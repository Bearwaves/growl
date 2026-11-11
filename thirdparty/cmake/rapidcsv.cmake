set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/rapidcsv)

growl_thirdparty_lib(rapidcsv
	INCLUDES INTERFACE "${SOURCE_PREFIX}/src"
)
