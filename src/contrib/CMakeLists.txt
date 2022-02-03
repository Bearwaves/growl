project(growl-contrib)

set(SOURCES
	"stb_image/stb_image.c"
	)

set(HEADERS
	"stb_image/stb_image.h"
	)

add_library(growl-contrib ${SOURCES} ${HEADERS})
add_subdirectory(glm)
target_link_libraries(growl-contrib glm::glm)
