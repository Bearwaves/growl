cmake_minimum_required (VERSION 3.19)

project(growl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(GROWL_DEFAULT_IMGUI ON)
set(GROWL_DEFAULT_DESKTOP ON)
set(GROWL_DEFAULT_IOS OFF)
set(GROWL_DEFAULT_TOOLS ON)

if (IOS)
	set(GROWL_DEFAULT_IMGUI OFF)
	set(GROWL_DEFAULT_DESKTOP OFF)
	set(GROWL_DEFAULT_IOS ON)
	set(GROWL_DEFAULT_TOOLS OFF)
endif()

# Note that the platforms turn on plugins according to their own logic.
# See src/platforms/*/CMakeLists.txt
option(GROWL_DESKTOP "Whether to build the Desktop platform" ${GROWL_DEFAULT_DESKTOP})
option(GROWL_IOS "Whether to build the iOS platform" ${GROWL_DEFAULT_IOS})
option(GROWL_OPENGL "Whether to build the OpenGL plugin" OFF)
option(GROWL_METAL "Whether to build the Metal plugin" OFF)
option(GROWL_SDL2 "Whether to build the SDL2 plugin" OFF)
option(GROWL_TOOLS "Whether to build the tools, e.g. growl-cmd" ${GROWL_DEFAULT_TOOLS})
option(GROWL_IMGUI "Whether to include Dear ImGui for debug UIs" ${GROWL_DEFAULT_IMGUI})

# Deal with third-party stuff that's in submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
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

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/thirdparty/json/CMakeLists.txt")
	message(FATAL_ERROR "[Growl] The submodules were not downloaded! GROWL_GIT_SUBMODULES was turned off or failed. Please update submodules and try again.")
endif()

add_subdirectory(thirdparty)
add_subdirectory(src)
