#include <growl/core/api/api.h>

using namespace Growl;

void API::assign() {
	if (systemInternal) {
		system = systemInternal.get();
	}
	if (graphicsInternal) {
		graphics = graphicsInternal.get();
	}
}
