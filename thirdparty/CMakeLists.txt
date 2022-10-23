cmake_minimum_required (VERSION 3.19)

project(growl-thirdparty)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(growl_thirdparty_lib name)
	cmake_parse_arguments(PARSE_ARGV 1 THIRDPARTY_PREFIX "INCLUDES" "" "SOURCES;PUBLIC;PRIVATE;DEFINITIONS;LINK")

	string(TOUPPER ${name} name_upper)
	set(lib "GROWL_THIRDPARTY_${name_upper}_LIBRARY")
	set(include_dir "GROWL_THIRDPARTY_${name_upper}_INCLUDE_DIR")

	if (DEFINED ${lib} AND DEFINED ${include_dir})
		message(STATUS "[Growl] Using user-provided ${name} at ${GROWL_THIRDPARTY_${name_upper}_LIBRARY}")
		add_library(${name} INTERFACE)
		target_link_libraries(${name} INTERFACE ${${lib}})
		target_include_directories(${name} INTERFACE ${${include_dir}})
	else ()
		add_library(${name} ${THIRDPARTY_PREFIX_SOURCES})
		target_include_directories(${name}
			PRIVATE ${THIRDPARTY_PREFIX_PRIVATE}
			PUBLIC ${THIRDPARTY_PREFIX_PUBLIC}
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
endif ()

if (GROWL_DESKTOP)
	add_compile_options(-msse4.1 -mpclmul)
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
add_library(growl-thirdparty::utf8cpp ALIAS utf8cpp)
add_library(growl-thirdparty::glm ALIAS glm)

include(cmake/harfbuzz.cmake)
include(cmake/libunibreak.cmake)
include(cmake/zlib.cmake)
include(cmake/freetype.cmake)
include(cmake/libpng.cmake)
include(cmake/msdfgen.cmake)
include(cmake/soloud.cmake)
include(cmake/fpng.cmake)
include(cmake/stb.cmake)

add_library(growl-thirdparty INTERFACE)
target_link_libraries(growl-thirdparty
	INTERFACE
	${THIRDPARTY_LINK_LIBRARIES}
	growl-thirdparty::glm
	growl-thirdparty::fmt
	growl-thirdparty::json
	growl-thirdparty::utf8cpp
	growl-thirdparty::freetype
	growl-thirdparty::harfbuzz
	growl-thirdparty::msdfgen
	growl-thirdparty::libunibreak
	growl-thirdparty::soloud
	growl-thirdparty::fpng
	growl-thirdparty::stb
	)

if (GROWL_IMGUI)
	include(cmake/dearimgui.cmake)
	target_link_libraries(growl-thirdparty INTERFACE growl-thirdparty::imgui)
endif()
