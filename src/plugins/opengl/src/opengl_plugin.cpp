#include "growl/core/api/api.h"
#include "opengl_graphics.h"

using Growl::API;
using Growl::OpenGLGraphicsAPI;

void initOpenGLPlugin(API& api) {
	api.addGraphicsAPI(std::make_unique<OpenGLGraphicsAPI>(api.system()));
}
