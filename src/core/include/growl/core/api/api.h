#pragma once

#include "api_internal.h"

#include <memory>

namespace Growl {

class GameAdapter;

class API {
	friend class GameAdapter;

public:
	SystemAPI& system() {
		return *systemInternal;
	}

	GraphicsAPI& graphics() {
		return *graphicsInternal;
	}

	AudioAPI& audio() {
		return *audioInternal;
	}

	void addSystemAPI(std::unique_ptr<SystemAPIInternal> internal);
	void addGraphicsAPI(std::unique_ptr<GraphicsAPIInternal> internal);
	void addAudioAPI(std::unique_ptr<AudioAPIInternal> internal);

private:
	std::unique_ptr<SystemAPIInternal> systemInternal;
	std::unique_ptr<GraphicsAPIInternal> graphicsInternal;
	std::unique_ptr<AudioAPIInternal> audioInternal;
};

} // namespace Growl
