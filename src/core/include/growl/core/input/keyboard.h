#pragma once

#include <string>

namespace Growl {

enum class KeyEventType {
	Unknown,

	KeyDown,
	KeyUp,
	TextInput
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

	FunctionF1,
	FunctionF2,
	FunctionF3,
	FunctionF4,
	FunctionF5,
	FunctionF6,
	FunctionF7,
	FunctionF8,
	FunctionF9,
	FunctionF10,
	FunctionF11,
	FunctionF12,
	FunctionF13,
	FunctionF14,
	FunctionF15,
	FunctionF16,
	FunctionF17,
	FunctionF18,
	FunctionF19,
	FunctionF20,
	FunctionF21,
	FunctionF22,
	FunctionF23,
	FunctionF24,

	Return,
	Space,
	Escape,
	Backspace,
};

struct InputKeyboardEvent {
	KeyEventType type;
	Key key;
	std::string text = "";
};

} // namespace Growl
