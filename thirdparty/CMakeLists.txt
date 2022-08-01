cmake_minimum_required (VERSION 3.19)

project(growl-thirdparty)

if (MSVC)
	set(CMAKE_CXX_FLAGS "/W0")
else ()
	set(CMAKE_CXX_FLAGS "-Wno-all -msse4.1 -mpclmul")
endif ()

# Get third-party stuff not to log
function(message)
	if (NOT MESSAGE_QUIET)
		_message(${ARGN})
	endif()
endfunction()

message(STATUS "[Growl] Third-party stuff, feel free to ignore any messages...")
set(MESSAGE_QUIET ON)

add_subdirectory(glm)
add_subdirectory(json)
add_subdirectory(fmt)
add_subdirectory(utfcpp)

unset(MESSAGE_QUIET)
message(STATUS "[Growl] All done with third-party.")

add_library(growl-thirdparty::json ALIAS nlohmann_json)
add_library(growl-thirdparty::fmt ALIAS fmt)

set(THIRDPARTY_SOURCES
	"fpng/fpng.cpp"

	"stb_image/stb_image.c"
	"stb_rect_pack/stb_rect_pack.c"

	"harfbuzz/src/harfbuzz.cc"
	)

set(THIRDPARTY_INCLUDES_PUBLIC)
set(THIRDPARTY_INCLUDES_PRIVATE)
set(THIRDPARTY_COMPILE_DEFINITIONS)

include(cmake/libunibreak.cmake)
include(cmake/zlib.cmake)
include(cmake/freetype.cmake)
include(cmake/libpng.cmake)
include(cmake/msdfgen.cmake)
include(cmake/soloud.cmake)

add_library(growl-thirdparty ${THIRDPARTY_SOURCES})
target_link_libraries(growl-thirdparty
	INTERFACE
	glm::glm
	growl-thirdparty::fmt
	growl-thirdparty::json
	utf8cpp
	)
target_include_directories(growl-thirdparty
	PRIVATE
	${THIRDPARTY_INCLUDES_PRIVATE}
	PUBLIC
	"harfbuzz/src"
	"fpng"
	"stb_image"
	"stb_rect_pack"
	${THIRDPARTY_INCLUDES_PUBLIC}
	)
target_compile_definitions(growl-thirdparty
	PRIVATE
	HAVE_FREETYPE
	"${THIRDPARTY_COMPILE_DEFINITIONS}"
	)
