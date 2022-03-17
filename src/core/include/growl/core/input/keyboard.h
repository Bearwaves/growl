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

struct InputKeyboardEvent {
	KeyEventType type;
	Key key;
};

} // namespace Growl
