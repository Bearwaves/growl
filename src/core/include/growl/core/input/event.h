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

struct InputMouseEvent {
	int mouseX;
	int mouseY;
};

struct InputControllerEvent {};

struct InputTouchEvent {};

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
