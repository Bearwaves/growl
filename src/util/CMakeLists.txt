project(growl-util)

include_directories("include")

set(SOURCES
	"src/image.cpp"
	)

set(HEADERS
	"include/growl/util/error.h"

	"include/growl/util/resources/image.h"
	)

add_library(growl-util ${SOURCES} ${HEADERS})
target_link_libraries(growl-util growl-contrib)
