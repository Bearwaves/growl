#include "growl/core/input/event.h"
#include "sdl_system.h"

using Growl::Key;
using Growl::KeyEventType;
using Growl::SDL2SystemAPI;

void SDL2SystemAPI::handleKeyboardEvent(SDL_Event& event) {
	if (inputProcessor) {
		InputEvent e(
			InputEventType::KEYBOARD,
			InputKeyboardEvent{getKeyEventType(event.key), getKey(event.key)});
		inputProcessor->onEvent(e);
	}
}

KeyEventType SDL2SystemAPI::getKeyEventType(SDL_KeyboardEvent& event) {
	switch (event.type) {
	case SDL_KEYDOWN:
		return KeyEventType::KeyDown;
	case SDL_KEYUP:
		return KeyEventType::KeyUp;
	default:
		return KeyEventType::Unknown;
	}
}

Key SDL2SystemAPI::getKey(SDL_KeyboardEvent& event) {
	switch (event.keysym.scancode) {
	// Arrows
	case SDL_SCANCODE_UP:
		return Key::ArrowUp;
	case SDL_SCANCODE_DOWN:
		return Key::ArrowDown;
	case SDL_SCANCODE_LEFT:
		return Key::ArrowLeft;
	case SDL_SCANCODE_RIGHT:
		return Key::ArrowRight;
		// Numbers
	case SDL_SCANCODE_0:
		return Key::Number0;
	case SDL_SCANCODE_1:
		return Key::Number1;
	case SDL_SCANCODE_2:
		return Key::Number2;
	case SDL_SCANCODE_3:
		return Key::Number3;
	case SDL_SCANCODE_4:
		return Key::Number4;
	case SDL_SCANCODE_5:
		return Key::Number5;
	case SDL_SCANCODE_6:
		return Key::Number6;
	case SDL_SCANCODE_7:
		return Key::Number7;
	case SDL_SCANCODE_8:
		return Key::Number8;
	case SDL_SCANCODE_9:
		return Key::Number9;

	// Letters
	case SDL_SCANCODE_A:
		return Key::LetterA;
	case SDL_SCANCODE_B:
		return Key::LetterB;
	case SDL_SCANCODE_C:
		return Key::LetterC;
	case SDL_SCANCODE_D:
		return Key::LetterD;
	case SDL_SCANCODE_E:
		return Key::LetterE;
	case SDL_SCANCODE_F:
		return Key::LetterF;
	case SDL_SCANCODE_G:
		return Key::LetterG;
	case SDL_SCANCODE_H:
		return Key::LetterH;
	case SDL_SCANCODE_I:
		return Key::LetterI;
	case SDL_SCANCODE_J:
		return Key::LetterJ;
	case SDL_SCANCODE_K:
		return Key::LetterK;
	case SDL_SCANCODE_L:
		return Key::LetterL;
	case SDL_SCANCODE_M:
		return Key::LetterM;
	case SDL_SCANCODE_N:
		return Key::LetterN;
	case SDL_SCANCODE_O:
		return Key::LetterO;
	case SDL_SCANCODE_P:
		return Key::LetterP;
	case SDL_SCANCODE_Q:
		return Key::LetterQ;
	case SDL_SCANCODE_R:
		return Key::LetterR;
	case SDL_SCANCODE_S:
		return Key::LetterS;
	case SDL_SCANCODE_T:
		return Key::LetterT;
	case SDL_SCANCODE_U:
		return Key::LetterU;
	case SDL_SCANCODE_V:
		return Key::LetterV;
	case SDL_SCANCODE_W:
		return Key::LetterW;
	case SDL_SCANCODE_X:
		return Key::LetterX;
	case SDL_SCANCODE_Y:
		return Key::LetterY;
	case SDL_SCANCODE_Z:
		return Key::LetterZ;

	case SDL_SCANCODE_F1:
		return Key::FunctionF1;
	case SDL_SCANCODE_F2:
		return Key::FunctionF2;
	case SDL_SCANCODE_F3:
		return Key::FunctionF3;
	case SDL_SCANCODE_F4:
		return Key::FunctionF4;
	case SDL_SCANCODE_F5:
		return Key::FunctionF5;
	case SDL_SCANCODE_F6:
		return Key::FunctionF6;
	case SDL_SCANCODE_F7:
		return Key::FunctionF7;
	case SDL_SCANCODE_F8:
		return Key::FunctionF8;
	case SDL_SCANCODE_F9:
		return Key::FunctionF9;
	case SDL_SCANCODE_F10:
		return Key::FunctionF10;
	case SDL_SCANCODE_F11:
		return Key::FunctionF11;
	case SDL_SCANCODE_F12:
		return Key::FunctionF12;
	case SDL_SCANCODE_F13:
		return Key::FunctionF13;
	case SDL_SCANCODE_F14:
		return Key::FunctionF14;
	case SDL_SCANCODE_F15:
		return Key::FunctionF15;
	case SDL_SCANCODE_F16:
		return Key::FunctionF16;
	case SDL_SCANCODE_F17:
		return Key::FunctionF17;
	case SDL_SCANCODE_F18:
		return Key::FunctionF18;
	case SDL_SCANCODE_F19:
		return Key::FunctionF19;
	case SDL_SCANCODE_F20:
		return Key::FunctionF20;
	case SDL_SCANCODE_F21:
		return Key::FunctionF21;
	case SDL_SCANCODE_F22:
		return Key::FunctionF22;
	case SDL_SCANCODE_F23:
		return Key::FunctionF23;
	case SDL_SCANCODE_F24:
		return Key::FunctionF24;

	default:
		return Key::Unknown;
	}
}
