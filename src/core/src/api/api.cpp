#include <growl/core/api/api.h>

using Growl::API;

void API::addAPI(API::Type apiType, APIInternal* internal) {
	switch (apiType) {
	case API::Type::System:
		systemInternal.reset(dynamic_cast<SystemAPIInternal*>(internal));
		return;
	case API::Type::Graphics:
		graphicsInternal.reset(dynamic_cast<GraphicsAPIInternal*>(internal));
		return;
	}
}
