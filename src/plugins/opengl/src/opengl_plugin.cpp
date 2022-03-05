#include "opengl_graphics.h"
#include <growl/core/api/api.h>

using Growl::API;
using Growl::OpenGLGraphicsAPI;

void initOpenGLPlugin(API& api) {
	api.addGraphicsAPI(std::make_unique<OpenGLGraphicsAPI>(*api.system()));
}
