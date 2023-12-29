cmake_minimum_required (VERSION 3.19)

project(growl-plugin-lua)

message(STATUS "[Growl] Enabled Lua plugin.")

set(SOURCES
	"src/lua_plugin.cpp"

	"src/lua_scripting.cpp"
	)

set(HEADERS
	"src/lua.h"

	"src/lua_scripting.h"
	"src/lua_script.h"
	)

add_library(growl-plugin-lua ${SOURCES} ${HEADERS})
target_link_libraries(growl-plugin-lua PRIVATE growl-core growl-thirdparty::lua)
