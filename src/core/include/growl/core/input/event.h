#pragma once

#include "keyboard.h"
#include <growl/util/error.h>
#include <string>
#include <variant>

namespace Growl {

enum class InputEventType {
	UNKNOWN,
	KEYBOARD,
	MOUSE,
	TOUCH,
	CONTROLLER,
	CUSTOM
};

class InputMouseEvent {
public:
	InputMouseEvent(int mouseX, int mouseY)
		: mouseX{mouseX}
		, mouseY{mouseY} {}
	int getMouseX() {
		return mouseX;
	}
	int getMouseY() {
		return mouseY;
	}

private:
	int mouseX;
	int mouseY;
};

class InputControllerEvent {};

class InputTouchEvent {};

class InputCustomEvent {};

struct InputEventError : public BaseError {
	std::string message() {
		return "Failed to get specific input event; wrong type?";
	}
};

using InputEventVariant = std::variant<
	InputCustomEvent, InputKeyboardEvent, InputControllerEvent, InputMouseEvent,
	InputTouchEvent>;

class InputEvent {
public:
	InputEvent(InputEventType type, InputEventVariant event)
		: type{type}
		, event{event} {}
	InputEventType getType() {
		return type;
	}
	template <class T>
	Result<T> getEvent() noexcept {
		if (std::holds_alternative<T>(event)) {
			return std::get<T>(event);
		}
		return Error(std::make_unique<InputEventError>());
	}

private:
	InputEventType type;
	InputEventVariant event;
};

} // namespace Growl
