#pragma once

#include "growl/core/api/system_api.h"
#include <growl/core/input/processor.h>

namespace Growl {

class InputHandler : public InputProcessor {
public:
	explicit InputHandler(SystemAPI* system)
		: system{system} {}

	void onMouseEvent(InputMouseEvent& event) override;

	int getMouseX() {
		return mouseX;
	}

	int getMouseY() {
		return mouseY;
	}

private:
	SystemAPI* system;
	int mouseX;
	int mouseY;
};

} // namespace Growl
