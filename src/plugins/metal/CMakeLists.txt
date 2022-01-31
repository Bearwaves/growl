project(growl-plugin-metal)

include_directories("../../core/include" ${SDL2_INCLUDE_DIR})

set(SOURCES
	"src/metal_plugin.mm"
	"src/metal_graphics.mm"
	)

set(HEADERS
	"src/metal_graphics.h"
	)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -x objective-c++")

add_library(growl-plugin-metal ${SOURCES} ${HEADERS})
find_library(FOUNDATION_LIBRARY Foundation)
find_library(METAL_LIBRARY Metal)
find_library(QUARTZ_LIBRARY QuartzCore)
target_link_libraries(
	growl-plugin-metal
	growl-core
	${FOUNDATION_LIBRARY} ${METAL_LIBRARY} ${QUARTZ_LIBRARY}
	)
