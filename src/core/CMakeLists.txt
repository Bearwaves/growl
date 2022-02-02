project(growl-core)

include_directories("include")

set(SOURCES
	"src/api/api.cpp"
	"src/game/game.cpp"
	)

set(HEADERS
	"include/growl/core/api/api.h"
	"include/growl/core/api/api_internal.h"
	"include/growl/core/api/system_api.h"
	"include/growl/core/api/graphics_api.h"

	"include/growl/core/game/game.h"

	"include/growl/core/graphics/batch.h"
	"include/growl/core/graphics/texture.h"
	"include/growl/core/graphics/window.h"
	)

add_library(growl-core ${SOURCES} ${HEADERS})
