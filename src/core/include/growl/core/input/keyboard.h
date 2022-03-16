#pragma once

namespace Growl {

enum class KeyEventType {
	Unknown,

	KeyDown,
	KeyUp
};

enum class Key {
	Unknown,

	ArrowUp,
	ArrowDown,
	ArrowLeft,
	ArrowRight
};

class InputKeyboardEvent {
public:
	InputKeyboardEvent(KeyEventType type, Key key)
		: type{type}
		, key{key} {}

	KeyEventType getType() {
		return type;
	}

	Key getKey() {
		return key;
	}

private:
	KeyEventType type;
	Key key;
};

} // namespace Growl
