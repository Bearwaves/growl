#include "sdl_system.h"
#include <growl/core/api/api.h>

using Growl::API;
using Growl::SDL2SystemAPI;

void initSDL2Plugin(API& api) {
	api.addSystemAPI(std::make_unique<SDL2SystemAPI>());
}
