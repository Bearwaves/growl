#pragma once

#include "growl/core/graphics/window.h"
#include <SDL.h>

namespace Growl {

class SDL2Window final : public Window {
public:
	explicit SDL2Window(SDL_Window* window);
	~SDL2Window();
	void* getNative() const override {
		return native;
	}

private:
	SDL_Window* native;
};

} // namespace Growl
