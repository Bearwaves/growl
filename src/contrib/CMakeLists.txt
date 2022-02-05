project(growl-contrib)

add_subdirectory(glm)

set(SOURCES
	"stb_image/stb_image.c"
	)

set(HEADERS
	"stb_image/stb_image.h"
	)

add_library(growl-contrib ${SOURCES} ${HEADERS})
target_link_libraries(growl-contrib glm::glm)
