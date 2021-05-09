project(growl-core)

include_directories("include")

set(SOURCES
	"src/game.cpp"
	)

set(HEADERS
	"include/growl/core/game.h"
	)

add_library(growl-core ${SOURCES} ${HEADERS})
