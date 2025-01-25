#include "growl/core/api/api.h"
#include "sdl3_system.h"
#include <memory>

using Growl::API;
using Growl::SDL3SystemAPI;

void initSDL3Plugin(API& api) {
	api.addSystemAPI(std::make_unique<SDL3SystemAPI>(api));
}
