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
	ArrowRight,

	Number0,
	Number1,
	Number2,
	Number3,
	Number4,
	Number5,
	Number6,
	Number7,
	Number8,
	Number9,

	LetterA,
	LetterB,
	LetterC,
	LetterD,
	LetterE,
	LetterF,
	LetterG,
	LetterH,
	LetterI,
	LetterJ,
	LetterK,
	LetterL,
	LetterM,
	LetterN,
	LetterO,
	LetterP,
	LetterQ,
	LetterR,
	LetterS,
	LetterT,
	LetterU,
	LetterV,
	LetterW,
	LetterX,
	LetterY,
	LetterZ,
};

struct InputKeyboardEvent {
	KeyEventType type;
	Key key;
};

} // namespace Growl
