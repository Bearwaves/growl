project(growl-util)

include_directories("include")

set(SOURCES
	"src/bundle.cpp"
	"src/image.cpp"
	"src/atlas.cpp"
	)

set(HEADERS
	"include/growl/util/error.h"

	"include/growl/util/assets/bundle.h"
	"include/growl/util/assets/error.h"
	"include/growl/util/assets/image.h"
	"include/growl/util/assets/atlas.h"
	)

add_library(growl-util ${SOURCES} ${HEADERS})
target_link_libraries(growl-util growl-contrib)
