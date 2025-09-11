cmake_minimum_required (VERSION 3.19)

project(growl-thirdparty)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(GROWL_THIRDPARTY_LOGGING "Whether to show CMake logs from thirdparty dependencies" OFF)

function(growl_thirdparty_lib name)
	cmake_parse_arguments(PARSE_ARGV 1 THIRDPARTY_PREFIX "INCLUDES" "" "SOURCES;PUBLIC;PRIVATE;INTERFACE;DEFINITIONS;LINK")

	string(TOUPPER ${name} name_upper)
	set(lib "GROWL_THIRDPARTY_${name_upper}_LIBRARY")
	set(include_dir "GROWL_THIRDPARTY_${name_upper}_INCLUDE_DIR")

	if (DEFINED ${lib} AND DEFINED ${include_dir})
		message(STATUS "[Growl] Using user-provided ${name} at ${GROWL_THIRDPARTY_${name_upper}_LIBRARY}")
		add_library(${name} INTERFACE)
		target_link_libraries(${name} INTERFACE ${${lib}})
		target_include_directories(${name} INTERFACE ${${include_dir}})
	else ()
		if (THIRDPARTY_PREFIX_SOURCES)
			add_library(${name} ${THIRDPARTY_PREFIX_SOURCES})
		else ()
			add_library(${name} INTERFACE)
		endif()
		target_include_directories(${name}
			PRIVATE ${THIRDPARTY_PREFIX_PRIVATE}
			PUBLIC ${THIRDPARTY_PREFIX_PUBLIC}
			INTERFACE ${THIRDPARTY_PREFIX_INTERFACE}
		)
		target_compile_definitions(${name} PRIVATE ${THIRDPARTY_PREFIX_DEFINITIONS})
		target_link_libraries(${name} PRIVATE ${THIRDPARTY_PREFIX_LINK})
		set_property(TARGET ${name} PROPERTY FOLDER "growl-thirdparty")
	endif()
	add_library(growl-thirdparty::${name} ALIAS ${name})
endfunction ()

if (MSVC)
	add_compile_options(/W0)
elseif (IOS)
	add_compile_options(-w)
	set(CMAKE_CXX_FLAGS "-x objective-c++")
else ()
	add_compile_options(-w)
endif()

if (GROWL_DESKTOP AND NOT MSVC AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
	add_compile_options(-msse4.1 -mpclmul)
endif()

# Get third-party stuff not to log
function(message)
	if (NOT MESSAGE_QUIET)
		_message(${ARGN})
	endif()
endfunction()

message(STATUS "[Growl] --- Third-party")

set(CMAKE_WARN_DEPRECATED ${GROWL_THIRDPARTY_LOGGING} CACHE BOOL "")

if (NOT GROWL_THIRDPARTY_LOGGING)
	set(MESSAGE_QUIET ON)
endif()

if (APPLE)
	set(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
endif()

message(STATUS "[Growl] --- glm")
add_subdirectory(glm)
message(STATUS "[Growl] --- json")
add_subdirectory(json)
message(STATUS "[Growl] --- fmt")
add_subdirectory(fmt)
message(STATUS "[Growl] --- utfcpp")
add_subdirectory(utfcpp)

add_library(growl-thirdparty::json ALIAS nlohmann_json)
add_library(growl-thirdparty::fmt ALIAS fmt)
add_library(growl-thirdparty::utf8cpp ALIAS utf8cpp)
add_library(growl-thirdparty::glm ALIAS glm)

message(STATUS "[Growl] --- harfbuzz")
include(cmake/harfbuzz.cmake)
message(STATUS "[Growl] --- libunibreak")
include(cmake/libunibreak.cmake)
message(STATUS "[Growl] --- zlib")
include(cmake/zlib.cmake)
message(STATUS "[Growl] --- freetype")
include(cmake/freetype.cmake)
message(STATUS "[Growl] --- libpng")
include(cmake/libpng.cmake)
message(STATUS "[Growl] --- msdfgen")
include(cmake/msdfgen.cmake)
message(STATUS "[Growl] --- fpng")
include(cmake/fpng.cmake)
message(STATUS "[Growl] --- stb")
include(cmake/stb.cmake)
message(STATUS "[Growl] --- date")
include(cmake/date.cmake)
message(STATUS "[Growl] --- libsodium")
include(cmake/libsodium.cmake)

add_library(growl-thirdparty INTERFACE)
target_link_libraries(growl-thirdparty
	INTERFACE
	growl-thirdparty::glm
	growl-thirdparty::fmt
	growl-thirdparty::json
	growl-thirdparty::utf8cpp
	growl-thirdparty::freetype
	growl-thirdparty::harfbuzz
	growl-thirdparty::msdfgen
	growl-thirdparty::libunibreak
	growl-thirdparty::zlib
	growl-thirdparty::fpng
	growl-thirdparty::stb
	growl-thirdparty::date
	growl-thirdparty::sodium
)

if (GROWL_SDL3)
	set(SDL_TEST_LIBRARY OFF)
	message(STATUS "[Growl] --- SDL")
	add_subdirectory(SDL)
	get_target_property(SDL_aliased SDL3::SDL3 ALIASED_TARGET)
	add_library(growl-thirdparty::sdl ALIAS ${SDL_aliased})
	target_link_libraries(growl-thirdparty INTERFACE growl-thirdparty::sdl)
	set_property(TARGET ${SDL_aliased} PROPERTY FOLDER "growl-thirdparty")
endif()

if (GROWL_IMGUI)
	message(STATUS "[Growl] --- imgui")
	include(cmake/dearimgui.cmake)
	target_link_libraries(growl-thirdparty INTERFACE growl-thirdparty::imgui)
endif()

if (GROWL_SOLOUD)
	message(STATUS "[Growl] --- soloud")
	include(cmake/soloud.cmake)
	target_link_libraries(growl-thirdparty INTERFACE growl-thirdparty::soloud)
endif()

if (GROWL_LUA)
	message(STATUS "[Growl] --- lua")
	include(cmake/lua.cmake)
	target_link_libraries(growl-thirdparty INTERFACE growl-thirdparty::lua)
endif()

if (GROWL_CURL)
	if (NOT WIN32)
		message(STATUS "[Growl] --- mbedtls")
		include(cmake/mbedtls.cmake)
		target_link_libraries(growl-thirdparty INTERFACE growl-thirdparty::mbedtls)
	endif()
	message(STATUS "[Growl] --- curl")
	include(cmake/curl.cmake)
	target_link_libraries(growl-thirdparty INTERFACE growl-thirdparty::curl)
endif()

unset(MESSAGE_QUIET)
unset(CMAKE_WARN_DEPRECATED CACHE)
message(STATUS "[Growl] --- All done with third-party.")
