#pragma once

#include "controller.h"
#include "growl/core/error.h"
#include "keyboard.h"
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

struct InputMouseEvent {
	int mouseX;
	int mouseY;
};

enum class TouchEventType { UP, DOWN, MOVE };

struct InputTouchEvent {
	TouchEventType type;
	int touchX;
	int touchY;
	int id;
};

struct InputCustomEvent {};

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
