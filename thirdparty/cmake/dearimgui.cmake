cmake_minimum_required (VERSION 3.19)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND NOT GROWL_OPENGL)
	project(growl-thirdparty-imgui LANGUAGES OBJCXX)
else()
	project(growl-thirdparty-imgui LANGUAGES CXX)
endif()

set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

set(SOURCES
	${SOURCE_PREFIX}/imgui.cpp
	${SOURCE_PREFIX}/imgui_tables.cpp
	${SOURCE_PREFIX}/imgui_widgets.cpp
	${SOURCE_PREFIX}/imgui_draw.cpp
	${SOURCE_PREFIX}/imgui_demo.cpp
	${SOURCE_PREFIX}/misc/cpp/imgui_stdlib.cpp
)

set(SOURCES
	${SOURCES}
	${SOURCE_PREFIX}/backends/imgui_impl_sdl3.cpp
)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND NOT GROWL_OPENGL)
	set(SOURCES
		${SOURCES}
		${SOURCE_PREFIX}/backends/imgui_impl_metal.mm
	)
else()
	set(SOURCES
		${SOURCES}
		${SOURCE_PREFIX}/backends/imgui_impl_opengl3.cpp
	)
endif()

if (GROWL_SDL3)
	set(SDL_LIB growl-thirdparty::sdl)
endif()

growl_thirdparty_lib(imgui
	SOURCES ${SOURCES}
	INCLUDES
		PUBLIC ${SOURCE_PREFIX} "${SOURCE_PREFIX}/backends" "${SOURCE_PREFIX}/misc/cpp"
	LINK ${SDL_LIB}
)
target_compile_definitions(imgui INTERFACE GROWL_IMGUI)
