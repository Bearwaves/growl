project(growl-contrib)

add_subdirectory(glm)

set(SOURCES
	"fmt/format.cpp"

	"stb_image/stb_image.c"
	)

set(HEADERS
	"fmt/core.h"
	"fmt/color.h"
	"fmt/format.h"
	"fmt/format-inl.h"

	"stb_image/stb_image.h"
	)

add_library(growl-contrib ${SOURCES} ${HEADERS})
target_link_libraries(growl-contrib glm::glm)
