cmake_minimum_required (VERSION 3.19)

project(growl-thirdparty)

include(FetchContent)

if (MSVC)
	set(CMAKE_CXX_FLAGS "/W0")
else ()
	set(CMAKE_CXX_FLAGS "-Wno-all -msse4.1 -mpclmul")
endif ()

add_subdirectory(glm)
add_subdirectory(utfcpp)
add_subdirectory(soloud)

# Get third-party stuff not to log
function(message)
	if (NOT MESSAGE_QUIET)
		_message(${ARGN})
	endif()
endfunction()

message(STATUS "Third-party stuff, feel free to ignore any messages...")
set(MESSAGE_QUIET ON)

FetchContent_Declare(
	json
	URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz
	)
FetchContent_Declare(
	fmt
	URL https://github.com/fmtlib/fmt/releases/download/8.1.1/fmt-8.1.1.zip
	)
FetchContent_Declare(
	freetype
	URL https://download.savannah.gnu.org/releases/freetype/freetype-2.12.0.tar.gz
	)
FetchContent_Declare(
	msdfgen
	URL https://github.com/Chlumsky/msdfgen/archive/refs/tags/v1.9.2.tar.gz
	)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(MSDFGEN_BUILD_STANDALONE OFF)
set(MSDFGEN_INSTALL OFF)
find_package(OpenMP)
if (OpenMP)
	set(MSDFGEN_USE_OPENMP ON)
endif()

FetchContent_MakeAvailable(json fmt freetype)
add_library(Freetype::Freetype ALIAS freetype) # Used by msdfgen
FetchContent_MakeAvailable(msdfgen)
unset(MESSAGE_QUIET)
message(STATUS "All done with third-party.")

add_library(growl-thirdparty::json ALIAS nlohmann_json)
add_library(growl-thirdparty::fmt ALIAS fmt)

set(SOURCES
	"fpng/fpng.cpp"

	"stb_image/stb_image.c"
	"stb_rect_pack/stb_rect_pack.c"

	"harfbuzz/src/harfbuzz.cc"

	"libunibreak/src/unibreakdef.c"
	"libunibreak/src/unibreakbase.c"
	"libunibreak/src/linebreakdef.c"
	"libunibreak/src/linebreak.c"
	"libunibreak/src/linebreakdata.c"
	)

set(HEADERS
	"fpng/fpng.h"

	"stb_image/stb_image.h"
	"stb_rect_pack/stb_rect_pack.h"

	"libunibreak/src/unibreakdef.h"
	"libunibreak/src/unibreakbase.h"
	"libunibreak/src/linebreakdef.h"
	"libunibreak/src/linebreak.h"
	)

add_library(growl-thirdparty ${SOURCES} ${HEADERS})
target_link_libraries(growl-thirdparty
	INTERFACE
	glm::glm
	growl-thirdparty::fmt
	growl-thirdparty::json
	utf8cpp
	soloud
	msdfgen-ext
	PUBLIC
	freetype
	)
target_include_directories(growl-thirdparty
	PUBLIC
	"harfbuzz/src"
	)
target_compile_definitions(growl-thirdparty
	PRIVATE
	"HAVE_FREETYPE"
	)
