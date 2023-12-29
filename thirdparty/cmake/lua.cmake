set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/lua)
set(SOURCES
	# https://www.lua.org/manual/5.4/readme.html
	"${SOURCE_PREFIX}/lapi.c"
	"${SOURCE_PREFIX}/lcode.c"
	"${SOURCE_PREFIX}/lctype.c"
	"${SOURCE_PREFIX}/ldebug.c"
	"${SOURCE_PREFIX}/ldo.c"
	"${SOURCE_PREFIX}/ldump.c"
	"${SOURCE_PREFIX}/lfunc.c"
	"${SOURCE_PREFIX}/lgc.c"
	"${SOURCE_PREFIX}/llex.c"
	"${SOURCE_PREFIX}/lmem.c"
	"${SOURCE_PREFIX}/lobject.c"
	"${SOURCE_PREFIX}/lopcodes.c"
	"${SOURCE_PREFIX}/lparser.c"
	"${SOURCE_PREFIX}/lstate.c"
	"${SOURCE_PREFIX}/lstring.c"
	"${SOURCE_PREFIX}/ltable.c"
	"${SOURCE_PREFIX}/ltm.c"
	"${SOURCE_PREFIX}/lundump.c"
	"${SOURCE_PREFIX}/lvm.c"
	"${SOURCE_PREFIX}/lzio.c"
	"${SOURCE_PREFIX}/lauxlib.c"
	"${SOURCE_PREFIX}/lbaselib.c"
	"${SOURCE_PREFIX}/lcorolib.c"
	"${SOURCE_PREFIX}/ldblib.c"
	"${SOURCE_PREFIX}/liolib.c"
	"${SOURCE_PREFIX}/lmathlib.c"
	"${SOURCE_PREFIX}/loadlib.c"
	"${SOURCE_PREFIX}/loslib.c"
	"${SOURCE_PREFIX}/lstrlib.c"
	"${SOURCE_PREFIX}/ltablib.c"
	"${SOURCE_PREFIX}/lutf8lib.c"
	"${SOURCE_PREFIX}/linit.c"
	)

set(INCLUDES
	"${SOURCE_PREFIX}"
	)

set(DEFINITIONS "")
if (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
	set(DEFINITIONS
		${DEFINITIONS}
		"LUA_USE_IOS"
		)
endif()

growl_thirdparty_lib(lua
	SOURCES	${SOURCES}
	INCLUDES PUBLIC ${INCLUDES}
	DEFINITIONS ${DEFINITIONS}
	)
