#pragma once

#include "growl/core/input/processor.h"

namespace Growl {

class SystemAPI;
class Node;

class InputHandler : public InputProcessor {
public:
	explicit InputHandler(SystemAPI& system, Node* root)
		: system{system}
		, root{root} {}

	void onEvent(InputEvent& event) override;

	void onMouseEvent(InputMouseEvent& event) override;

	void onKeyboardEvent(InputKeyboardEvent& event) override;

	void onControllerEvent(InputControllerEvent& event) override;

	void onTouchEvent(InputTouchEvent& event) override;

	int getMouseX() {
		return mouseX;
	}

	int getMouseY() {
		return mouseY;
	}

	bool upPressed() {
		return up;
	}

	bool downPressed() {
		return down;
	}

	bool leftPressed() {
		return left;
	}

	bool rightPressed() {
		return right;
	}

	bool clockwisePressed() {
		return clockwise;
	}

	bool anticlockwisePressed() {
		return anticlockwise;
	}

private:
	SystemAPI& system;
	Node* root;
	int mouseX;
	int mouseY;
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool clockwise = false;
	bool anticlockwise = false;
};

} // namespace Growl
