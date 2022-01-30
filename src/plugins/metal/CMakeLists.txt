project(growl-plugin-metal)

include_directories("include" "../../core/include")

set(SOURCES
	"src/metal_graphics.mm"
	)

set(HEADERS
	"include/growl/plugins/metal/metal_graphics.h"
	)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -x objective-c++")

add_library(growl-plugin-metal ${SOURCES} ${HEADERS})
find_library(METAL_LIBRARY Metal)
find_library(QUARTZ_LIBRARY QuartzCore)
target_link_libraries(growl-plugin-metal growl-core ${METAL_LIBRARY} ${QUARTZ_LIBRARY})
