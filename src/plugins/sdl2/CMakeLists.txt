project(growl-plugin-sdl2)

set(SDL2_BUILDING_LIBRARY 1)
find_Package(SDL2 REQUIRED)

include_directories("include" "../../core/include" ${SDL2_INCLUDE_DIR})

set(SOURCES
	"src/sdl_system.cpp"
	)

set(HEADERS
	"include/growl/plugins/sdl2/sdl_system.h"
	)

add_library(growl-plugin-sdl2 ${SOURCES} ${HEADERS})
target_link_libraries(growl-plugin-sdl2 growl-core ${SDL2_LIBRARIES})
