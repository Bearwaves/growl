#include <growl/core/api/api.h>

using Growl::API;

void API::assign() {
	if (systemInternal) {
		system = systemInternal.get();
	}
	if (graphicsInternal) {
		graphics = graphicsInternal.get();
	}
}
