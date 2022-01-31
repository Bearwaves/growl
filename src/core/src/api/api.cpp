#include <growl/core/api/api.h>

using Growl::API;

void API::addSystemAPI(SystemAPIInternal* internal) {
	systemInternal.reset(internal);
}

void API::addGraphicsAPI(GraphicsAPIInternal* internal) {
	graphicsInternal.reset(internal);
}
