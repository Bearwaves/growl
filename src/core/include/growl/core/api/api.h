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
	std::unique_ptr<SystemAPIInternal> systemInternal;
	std::unique_ptr<GraphicsAPIInternal> graphicsInternal;
};

} // namespace Growl
