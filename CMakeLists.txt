cmake_minimum_required (VERSION 3.19)

project(growl)

set (GROWL_VERSION 0.1.0-PREVIEW)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GROWL_DEFAULT_IMGUI ON)
set(GROWL_DEFAULT_DESKTOP ON)
set(GROWL_DEFAULT_IOS OFF)
set(GROWL_DEFAULT_ANDROID OFF)
set(GROWL_DEFAULT_WEB OFF)
set(GROWL_DEFAULT_TOOLS ON)
set(GROWL_DEFAULT_SDL3 ON)
set(GROWL_DEFAULT_CURL ON)

if (IOS)
	set(GROWL_DEFAULT_IMGUI OFF)
	set(GROWL_DEFAULT_DESKTOP OFF)
	set(GROWL_DEFAULT_IOS ON)
	set(GROWL_DEFAULT_TOOLS OFF)
	set(GROWL_DEFAULT_SDL3 OFF)
	set(GROWL_DEFAULT_HTTPLIB OFF)
	set(GROWL_DEFAULT_CURL OFF)
endif()

if (ANDROID)
	set(GROWL_DEFAULT_IMGUI OFF)
	set(GROWL_DEFAULT_DESKTOP OFF)
	set(GROWL_DEFAULT_ANDROID ON)
	set(GROWL_DEFAULT_TOOLS OFF)
	set(GROWL_DEFAULT_SDL3 OFF)
	set(GROWL_DEFAULT_HTTPLIB OFF)
	add_compile_options(-fPIC)
endif()

if (APPLE)
	enable_language(OBJC)
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	set(GROWL_DEFAULT_DESKTOP OFF)
	set(GROWL_DEFAULT_TOOLS OFF)
	set(GROWL_DEFAULT_WEB ON)
	set(GROWL_DEFAULT_CURL OFF)
endif()

# Note that the platforms turn on plugins according to their own logic.
# See src/platforms/*/CMakeLists.txt
option(GROWL_DESKTOP "Whether to build the Desktop platform" ${GROWL_DEFAULT_DESKTOP})
option(GROWL_IOS "Whether to build the iOS platform" ${GROWL_DEFAULT_IOS})
option(GROWL_ANDROID "Whether to build the Android platform" ${GROWL_DEFAULT_ANDROID})
option(GROWL_WEB "Whether to build the Web platform" ${GROWL_DEFAULT_WEB})
option(GROWL_OPENGL "Whether to build the OpenGL plugin" OFF)
option(GROWL_METAL "Whether to build the Metal plugin" OFF)
option(GROWL_SDL3 "Whether to build the SDL3 plugin" ${GROWL_DEFAULT_SDL3})
option(GROWL_SOLOUD "Whether to build the SoLoud plugin" ON)
option(GROWL_TOOLS "Whether to build the tools, e.g. growl-cmd" ${GROWL_DEFAULT_TOOLS})
option(GROWL_IMGUI "Whether to include Dear ImGui for debug UIs" ${GROWL_DEFAULT_IMGUI})
option(GROWL_LUA "Whether to include Lua for scripting" ON)
option(GROWL_CURL "Whether to build the curl library" ${GROWL_DEFAULT_CURL})

# Deal with third-party stuff that's in submodules
find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	option(GROWL_GIT_SUBMODULES "Check submodules during build" ON)
	if(GROWL_GIT_SUBMODULES)
		message(STATUS "[Growl] Fetching submodules")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
										WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
										RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "[Growl] git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
		endif()
	endif()
endif()

if (NOT EXISTS "${PROJECT_SOURCE_DIR}/thirdparty/json/CMakeLists.txt")
	message(FATAL_ERROR "[Growl] The submodules were not downloaded! GROWL_GIT_SUBMODULES was turned off or failed. Please update submodules and try again.")
endif()

add_subdirectory(thirdparty)
add_subdirectory(src)
