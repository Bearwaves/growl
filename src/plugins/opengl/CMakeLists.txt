cmake_minimum_required (VERSION 3.19)

project(growl-plugin-opengl)

message(STATUS "[Growl] Enabled OpenGL plugin.")

option(GROWL_FORCE_OPENGL_4_1 "Whether to force OpenGL 4.1 (older) regardless of platform" OFF)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR GROWL_FORCE_OPENGL_4_1)
	set(GROWL_OPENGL_4_1 1)
	add_definitions(-DGROWL_OPENGL_4_1)
	message(STATUS "[Growl] Building with OpenGL 4.1")
elseif (ANDROID OR ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	set(GROWL_OPENGL_ES 1)
	add_definitions(-DGROWL_OPENGL_ES)
	message(STATUS "[Growl] Building with OpenGL ES")
else ()
	set(GROWL_OPENGL_4_5 1)
	add_definitions(-DGROWL_OPENGL_4_5)
	message(STATUS "[Growl] Building with OpenGL 4.5")
endif()

if (ANDROID)
	find_path(OPENGL_INCLUDE_DIRS GLES3/gl3.h HINTS ${ANDROID_NDK})
	find_library(OPENGL_LIBRARIES libGLESv3.so HINTS ${ANDROID_NDK})
else ()
	set(OpenGL_GL_PREFERENCE GLVND)
	find_package(OpenGL REQUIRED)
endif()

set(SOURCES
	"src/opengl_graphics.cpp"
	"src/opengl_batch.cpp"
	"src/opengl_texture.cpp"
	"src/opengl_shader.cpp"
)

set(HEADERS
	"src/opengl.h"

	"src/opengl_graphics.h"
	"src/opengl_batch.h"
	"src/opengl_texture.h"
	"src/opengl_shader.h"
	"src/opengl_error.h"
)

if (GROWL_OPENGL_4_5)
	set(SOURCES
		"thirdparty/opengl_4_5.c"
		${SOURCES}
	)
	set(HEADERS
		"thirdparty/opengl_4_5.h"
		"thirdparty/khrplatform.h"
		${HEADERS}
	)
endif()

add_library(growl-plugin-opengl ${SOURCES} ${HEADERS})
target_link_libraries(
	growl-plugin-opengl
	PRIVATE
	growl-core
	growl-thirdparty
	${OPENGL_LIBRARIES}
)
target_compile_definitions(growl-plugin-opengl INTERFACE "GROWL_OPENGL")
target_include_directories(growl-plugin-opengl PRIVATE ${OPENGL_INCLUDE_DIRS})
