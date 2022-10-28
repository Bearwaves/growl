set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/harfbuzz)

growl_thirdparty_lib(harfbuzz
	SOURCES "${SOURCE_PREFIX}/src/harfbuzz.cc"
	INCLUDES PUBLIC "${SOURCE_PREFIX}/src"
	DEFINITIONS HAVE_FREETYPE
	LINK growl-thirdparty::freetype
	)
