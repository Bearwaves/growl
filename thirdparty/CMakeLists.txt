cmake_minimum_required (VERSION 3.19)

project(growl-thirdparty)

include(FetchContent)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0042 NEW)

if (MSVC)
	set(CMAKE_CXX_FLAGS "/W0")
else ()
	set(CMAKE_CXX_FLAGS "-Wno-all -msse4.1 -mpclmul")
endif ()

add_subdirectory(glm)
add_subdirectory(json)
add_subdirectory(zlib)
add_subdirectory(soloud)

# Get third-party stuff not to log
function(message)
	if (NOT MESSAGE_QUIET)
		_message(${ARGN})
	endif()
endfunction()

message(STATUS "[Growl] Third-party stuff, feel free to ignore any messages...")
set(MESSAGE_QUIET ON)

FetchContent_Declare(
	fmt
	URL https://github.com/fmtlib/fmt/releases/download/8.1.1/fmt-8.1.1.zip
	)
FetchContent_Declare(
	libpng
	URL https://github.com/glennrp/libpng/archive/refs/tags/v1.6.35.tar.gz
	)
FetchContent_Declare(
	freetype
	URL https://download.savannah.gnu.org/releases/freetype/freetype-2.12.0.tar.gz
	)
FetchContent_Declare(
	msdfgen
	URL https://github.com/Chlumsky/msdfgen/archive/refs/tags/v1.9.2.tar.gz
	)
FetchContent_Declare(
	utf8cpp
	URL https://github.com/nemtrif/utfcpp/archive/refs/tags/v3.2.1.tar.gz
	)

# Stop FreeType/libpng from finding these in system paths.
set(FT_DISABLE_ZLIB ON)
set(FT_DISABLE_HARFBUZZ ON)
set(FT_DISABLE_PNG ON)
set(FT_DISABLE_BZIP2 ON)
set(FT_DISABLE_BROTLI ON)
set(PNG_SHARED OFF)
set(PNG_TESTS OFF)
set(PNG_BUILD_ZLIB ON)
set(SKIP_INSTALL_ALL ON)

FetchContent_MakeAvailable(fmt libpng freetype utf8cpp)

target_link_libraries(png_static zlib)
target_link_libraries(freetype PRIVATE zlib)

target_compile_definitions(freetype
	PRIVATE
	FT_CONFIG_OPTION_ERROR_STRINGS
	FT_CONFIG_OPTION_USE_HARFBUZZ
	FT_CONFIG_OPTION_USE_PNG
	FT_CONFIG_OPTION_SYSTEM_ZLIB
	)
target_include_directories(freetype
	PRIVATE
	"harfbuzz/src"
	${libpng_SOURCE_DIR}
	${libpng_BINARY_DIR}
	)

set(MSDFGEN_BUILD_STANDALONE OFF)
set(MSDFGEN_INSTALL OFF)
find_package(OpenMP)
if (OpenMP)
	set(MSDFGEN_USE_OPENMP ON)
endif()

add_library(Freetype::Freetype ALIAS freetype) # Used by msdfgen
FetchContent_MakeAvailable(msdfgen)
unset(MESSAGE_QUIET)
message(STATUS "[Growl] All done with third-party.")

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

add_library(growl-thirdparty ${SOURCES})
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
	PRIVATE
	png_static
	)
target_include_directories(growl-thirdparty
	PUBLIC
	"harfbuzz/src"
	"libunibreak/src"
	"fpng"
	"stb_image"
	"stb_rect_pack"
	)
target_compile_definitions(growl-thirdparty
	PRIVATE
	"HAVE_FREETYPE"
	)
