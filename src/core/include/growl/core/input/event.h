#pragma once

#include "controller.h"
#include "growl/core/error.h"
#include "keyboard.h"
#include <string>
#include <variant>

namespace Growl {

enum class InputEventType {
	Unknown,
	Keyboard,
	Mouse,
	Touch,
	Controller,
	Custom
};

enum class PointerEventType { Unknown, Up, Down, Move };

enum class MouseButton { Unknown, LeftClick, RightClick, MiddleClick };

struct InputMouseEvent {
	PointerEventType type;
	MouseButton button;
	int mouseX;
	int mouseY;
};

struct InputTouchEvent {
	PointerEventType type;
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
	InputEventType getType() const {
		return type;
	}
	template <class T>
	Result<T> getEvent() const noexcept {
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
