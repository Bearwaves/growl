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

	bool onEvent(const InputEvent& event) override;

	bool onKeyboardEvent(const InputKeyboardEvent& event) override;

	bool onControllerEvent(const InputControllerEvent& event) override;

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
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool clockwise = false;
	bool anticlockwise = false;
};

} // namespace Growl
