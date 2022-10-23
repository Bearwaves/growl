cmake_minimum_required (VERSION 3.19)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND NOT GROWL_OPENGL)
	project(growl-thirdparty-imgui LANGUAGES OBJCXX)
else()
	project(growl-thirdparty-imgui LANGUAGES CXX)
endif()

set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

set(IMGUI_SOURCES
	${SOURCE_PREFIX}/imgui.cpp
	${SOURCE_PREFIX}/imgui_tables.cpp
	${SOURCE_PREFIX}/imgui_widgets.cpp
	${SOURCE_PREFIX}/imgui_draw.cpp
	${SOURCE_PREFIX}/imgui_demo.cpp
	)

set(IMGUI_BACKEND_SOURCES
	${SOURCE_PREFIX}/backends/imgui_impl_sdl.cpp
	)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND NOT GROWL_OPENGL)
	set(IMGUI_BACKEND_SOURCES
		${IMGUI_BACKEND_SOURCES}
		${SOURCE_PREFIX}/backends/imgui_impl_metal.mm
		)
else()
	set(IMGUI_BACKEND_SOURCES
		${IMGUI_BACKEND_SOURCES}
		${SOURCE_PREFIX}/backends/imgui_impl_opengl3.cpp
		)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	set(SDL2_INCLUDE_DIR "")
	target_compile_options(imgui PRIVATE -sUSE_SDL=2)
else ()
	set(SDL2_BUILDING_LIBRARY 1)
	find_package(SDL2 REQUIRED)
endif ()

growl_thirdparty_lib(imgui
	SOURCES ${IMGUI_SOURCES} ${IMGUI_BACKEND_SOURCES}
	INCLUDES
		PUBLIC ${SOURCE_PREFIX} "${SOURCE_PREFIX}/backends"
		PRIVATE ${SDL2_INCLUDE_DIR}
	LINK ${SDL2_LIBRARY}
	)
target_compile_definitions(imgui INTERFACE GROWL_IMGUI)
