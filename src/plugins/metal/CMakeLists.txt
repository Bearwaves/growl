cmake_minimum_required (VERSION 3.19)

project(growl-plugin-metal LANGUAGES OBJCXX)

message(STATUS "[Growl] Enabled Metal plugin.")

set(SOURCES
	"src/metal_plugin.mm"
	"src/metal_graphics.mm"
	"src/metal_texture.mm"
	"src/metal_batch.mm"
	"src/metal_shader.mm"
	)

set(HEADERS
	"src/metal_graphics.h"
	"src/metal_texture.h"
	"src/metal_batch.h"
	"src/metal_shader.h"
	)

add_library(growl-plugin-metal ${SOURCES} ${HEADERS})
target_link_libraries(
	growl-plugin-metal
	PRIVATE
	growl-core
	growl-thirdparty
	"-framework Foundation"
	"-framework Metal"
	"-framework QuartzCore"
	)
target_compile_definitions(growl-plugin-metal INTERFACE "GROWL_METAL")
