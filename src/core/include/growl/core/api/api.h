#pragma once

#include "api_internal.h"

#include <memory>

namespace Growl {

class GameAdapter;

class API {

	friend class GameAdapter;

public:
	SystemAPI* system() {
		return systemInternal.get();
	}
	GraphicsAPI* graphics() {
		return graphicsInternal.get();
	}

private:
	std::shared_ptr<SystemAPIInternal> systemInternal;
	std::shared_ptr<GraphicsAPIInternal> graphicsInternal;
};

} // namespace Growl
