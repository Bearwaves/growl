project(growl-core)

include_directories("include")

set(SOURCES
	"src/api/api.cpp"
	"src/game/game.cpp"
	"src/input/processor.cpp"
	)

set(HEADERS
	"include/growl/core/log.h"

	"include/growl/core/api/api.h"
	"include/growl/core/api/api_internal.h"
	"include/growl/core/api/system_api.h"
	"include/growl/core/api/graphics_api.h"

	"include/growl/core/game/game.h"

	"include/growl/core/graphics/batch.h"
	"include/growl/core/graphics/texture.h"
	"include/growl/core/graphics/window.h"

	"include/growl/core/input/event.h"
	"include/growl/core/input/processor.h"
	"include/growl/core/input/controller.h"
	"include/growl/core/input/keyboard.h"
	)

add_library(growl-core ${SOURCES} ${HEADERS})
