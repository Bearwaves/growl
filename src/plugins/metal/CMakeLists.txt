cmake_minimum_required (VERSION 3.19)

project(growl-plugin-metal LANGUAGES OBJCXX)

set(SOURCES
	"src/metal_plugin.mm"
	"src/metal_graphics.mm"
	"src/metal_texture.mm"
	"src/metal_texture_atlas.mm"
	"src/metal_batch.mm"
	"src/metal_shader.mm"
	)

set(HEADERS
	"src/metal_graphics.h"
	"src/metal_texture.h"
	"src/metal_texture_atlas.h"
	"src/metal_batch.h"
	"src/metal_shader.h"
	)

add_library(growl-plugin-metal ${SOURCES} ${HEADERS})
target_link_libraries(
	growl-plugin-metal
	PRIVATE
	growl-core
	growl-plugin-sdl2
	"-framework Foundation"
	"-framework Metal"
	"-framework QuartzCore"
	)
