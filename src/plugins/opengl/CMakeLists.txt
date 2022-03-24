project(growl-plugin-opengl)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

include_directories("../../core/include" ${SDL2_INCLUDE_DIR})

set(SOURCES
	"src/opengl_plugin.cpp"

	"src/opengl_graphics.cpp"
	"src/opengl_batch.cpp"
	"src/opengl_texture.cpp"
	"src/opengl_texture_atlas.cpp"
	"src/opengl_shader.cpp"
	)

set(HEADERS
	"src/opengl_graphics.h"
	"src/opengl_batch.h"
	"src/opengl_texture.h"
	"src/opengl_texture_atlas.h"
	"src/opengl_shader.h"
	)

add_library(growl-plugin-opengl ${SOURCES} ${HEADERS})
target_link_libraries(
	growl-plugin-opengl
	growl-core
	growl-contrib
	${OPENGL_LIBRARIES}
	)
