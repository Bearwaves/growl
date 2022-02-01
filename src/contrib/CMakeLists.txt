project(growl-contrib)

set(SOURCES
	"stb_image/stb_image.c"
	)

set(HEADERS
	"stb_image/stb_image.h"
	)

add_library(growl-contrib ${SOURCES} ${HEADERS})
