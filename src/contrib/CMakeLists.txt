project(growl-contrib)

add_subdirectory(glm)
add_subdirectory(json)

set(CMAKE_CXX_FLAGS "-msse4.1 -mpclmul")

set(SOURCES
	"fmt/format.cpp"

	"fpng/fpng.cpp"

	"stb_image/stb_image.c"
	)

set(HEADERS
	"fmt/core.h"
	"fmt/color.h"
	"fmt/format.h"
	"fmt/format-inl.h"

	"fpng/fpng.h"

	"stb_image/stb_image.h"
	)

add_library(growl-contrib ${SOURCES} ${HEADERS})
target_link_libraries(growl-contrib glm::glm nlohmann_json::nlohmann_json)
