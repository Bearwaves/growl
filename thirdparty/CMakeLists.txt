cmake_minimum_required (VERSION 3.19)

project(growl-thirdparty)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(growl_thirdparty_lib name sources includes)
	string(TOUPPER ${name} name_upper)
	set(lib "GROWL_THIRDPARTY_${name_upper}_LIBRARY")
	set(include_dir "GROWL_THIRDPARTY_${name_upper}_INCLUDE_DIR")
	if (DEFINED ${lib} AND DEFINED ${include_dir})
		message(STATUS "[Growl] Using user-provided ${name} at ${GROWL_THIRDPARTY_${name_upper}_LIBRARY}")
		add_library(${name} INTERFACE)
		target_link_libraries(${name} INTERFACE ${${lib}})
		target_include_directories(${name} INTERFACE ${${include_dir}})
	else ()
		add_library(${name} ${sources})
		target_include_directories(${name} PUBLIC ${includes})
		set_property(TARGET ${name} PROPERTY FOLDER "thirdparty")
	endif()
	add_library(growl-thirdparty::${name} ALIAS ${name})
endfunction ()

if (MSVC)
	add_compile_options(/W0)
elseif (IOS)
	add_compile_options(-w)
	set(CMAKE_CXX_FLAGS "-x objective-c++")
elseif (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	add_compile_options(-w)
else()
	add_compile_options(-w -msse4.1 -mpclmul)
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

set(THIRDPARTY_LINK_LIBRARIES
	glm::glm
	growl-thirdparty::fmt
	growl-thirdparty::json
	utf8cpp
	)

set(THIRDPARTY_LINK_LIBRARIES_PRIVATE)
set(THIRDPARTY_INCLUDES_PUBLIC)
set(THIRDPARTY_INCLUDES_PRIVATE)
set(THIRDPARTY_COMPILE_DEFINITIONS)

if (GROWL_IMGUI)
	include(cmake/dearimgui.cmake)
	add_library(growl-thirdparty::imgui ALIAS imgui)
	set(THIRDPARTY_LINK_LIBRARIES
		${THIRDPARTY_LINK_LIBRARIES}
		growl-thirdparty::imgui
		)
endif()

if (GROWL_IOS)
	set(THIRDPARTY_LINK_LIBRARIES_PRIVATE
		${THIRDPARTY_LINK_LIBRARIES_PRIVATE}
		"-framework CoreFoundation"
		"-framework AVFAudio"
		"-framework AudioToolbox"
		)
endif()

set(THIRDPARTY_SOURCES
	"fpng/fpng.cpp"

	"stb_image/stb_image.c"
	"stb_rect_pack/stb_rect_pack.c"
	)

include(cmake/harfbuzz.cmake)
include(cmake/libunibreak.cmake)
include(cmake/zlib.cmake)
include(cmake/freetype.cmake)
include(cmake/libpng.cmake)
include(cmake/msdfgen.cmake)
include(cmake/soloud.cmake)

add_library(growl-thirdparty ${THIRDPARTY_SOURCES})
target_link_libraries(growl-thirdparty
	INTERFACE
	${THIRDPARTY_LINK_LIBRARIES}
	PRIVATE
	${THIRDPARTY_LINK_LIBRARIES_PRIVATE}
	growl-thirdparty::libpng
	)
target_include_directories(growl-thirdparty
	PRIVATE
	${THIRDPARTY_INCLUDES_PRIVATE}
	PUBLIC
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
