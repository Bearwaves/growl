cmake_minimum_required (VERSION 3.20)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(growl-example-ui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
	"src/main.cpp"
	"src/game.cpp"
	)

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /permissive-")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
	set(CMAKE_OBJCXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif ()

set(GROWL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)
add_subdirectory(${GROWL_PATH} ${CMAKE_CURRENT_BINARY_DIR}/growl)

if (GROWL_ANDROID)
	add_library(growl-example-ui SHARED ${SOURCES} ${HEADERS})
else ()
	add_executable(growl-example-ui ${SOURCES} ${HEADERS})
endif ()

if (GROWL_DESKTOP)
	target_link_libraries(growl-example-ui PRIVATE growl-platform-desktop)
elseif (GROWL_IOS)
	target_link_libraries(growl-example-ui PRIVATE growl-platform-ios)
	target_compile_options(growl-example-ui PRIVATE -x objective-c++)
elseif (GROWL_ANDROID)
	target_link_libraries(growl-example-ui PUBLIC growl-platform-android)
elseif (GROWL_WEB)
	target_link_libraries(growl-example-ui PRIVATE growl-platform-web)
	target_link_options(growl-example-ui PRIVATE --preload-file assets.growl)
	set(CMAKE_EXECUTABLE_SUFFIX .html)
	# We need to use this over the more modern target_link_options as CMake
	# currently struggles with dollar signs in linker flags.
	set(CMAKE_EXE_LINKER_FLAGS
		"${CMAKE_EXE_LINKER_FLAGS} -sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=$ccall"
		)
endif()
