#include <growl/core/api/api.h>
#include <growl/plugins/sdl2/sdl_system.h>

using Growl::API;
using Growl::SDL2SystemAPI;

void initSDL2Plugin(API& api) {
	api.addSystemAPI(std::make_unique<SDL2SystemAPI>());
}
