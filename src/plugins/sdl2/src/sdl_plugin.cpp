#include "growl/core/api/api.h"
#include "sdl_audio.h"
#include "sdl_system.h"
#include <memory>

using Growl::API;
using Growl::SDL2AudioAPI;
using Growl::SDL2SystemAPI;

void initSDL2Plugin(API& api) {
	api.addSystemAPI(std::make_unique<SDL2SystemAPI>());
	api.addAudioAPI(std::make_unique<SDL2AudioAPI>(api.system()));
}
