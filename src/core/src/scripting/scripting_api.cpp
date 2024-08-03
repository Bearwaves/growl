#include "growl/core/api/scripting_api.h"
#include "growl/core/api/api.h"
#include "growl/core/error.h"
#include "growl/core/scripting/class.h"

using Growl::API;
using Growl::Class;
using Growl::Error;
using Growl::ScriptingAPI;
using Growl::SystemAPI;

Error ScriptingAPI::mountGrowlScripts(API& api) {
	// Growl static class
	auto growl_class_result = createClass("Growl", true);
	if (!growl_class_result) {
		return std::move(growl_class_result.error());
	}

	if (auto err =
			growl_class_result.get()
				->addMethod<void, std::string_view, std::string_view>(
					"log",
					[](ClassSelf* self, void* ctx,
					   const std::vector<ScriptingParam>& args)
						-> Result<ScriptingParam> {
						SystemAPI* system = static_cast<SystemAPI*>(ctx);
						auto& tag = std::get<std::string_view>(args.at(0));
						auto& msg = std::get<std::string_view>(args.at(1));
						system->log(std::string("lua::").append(tag), msg);
						return ScriptingParam();
					},
					&(api.system()))) {
		return err;
	}

	if (auto err = growl_class_result.get()->addMethod<float>(
			"getDeltaTime",
			[](ClassSelf* self, void* ctx,
			   const std::vector<ScriptingParam>& args)
				-> Result<ScriptingParam> {
				GraphicsAPI* graphics = static_cast<GraphicsAPI*>(ctx);
				return ScriptingParam(
					static_cast<float>(graphics->getDeltaTime()));
			},
			&(api.graphics()))) {
		return err;
	}

	// Input events
	auto input_mouse_event_res = createClass("InputMouseEvent", false);
	if (!input_mouse_event_res) {
		return std::move(input_mouse_event_res.error());
	}
	auto& input_mouse_event_cls = *input_mouse_event_res;

	if (auto err =
			input_mouse_event_cls->addConstructor<const InputMouseEvent*>(
				[](ClassSelf* self, void* ctx,
				   const std::vector<ScriptingParam>& args)
					-> Result<ScriptingParam> {
					auto event = static_cast<const InputMouseEvent*>(
						std::get<const void*>(args.at(0)));
					self->setField("button", (int)event->button);
					self->setField("mouseX", event->mouseX);
					self->setField("mouseY", event->mouseY);
					self->setField("type", (int)event->type);
					return ScriptingParam();
				},
				nullptr);
		err) {
		return err;
	}

	auto input_keyboard_event_res = createClass("InputKeyboardEvent", false);
	if (!input_keyboard_event_res) {
		return std::move(input_keyboard_event_res.error());
	}
	auto& input_keyboard_event_cls = *input_keyboard_event_res;

	if (auto err =
			input_keyboard_event_cls->addConstructor<const InputKeyboardEvent*>(
				[](ClassSelf* self, void* ctx,
				   const std::vector<ScriptingParam>& args)
					-> Result<ScriptingParam> {
					auto event = static_cast<const InputKeyboardEvent*>(
						std::get<const void*>(args.at(0)));
					self->setField("type", (int)event->type);
					self->setField("key", (int)event->key);
					return ScriptingParam();
				},
				nullptr)) {
		return err;
	}

	auto input_touch_event_res = createClass("InputTouchEvent", false);
	if (!input_touch_event_res) {
		return std::move(input_touch_event_res.error());
	}
	auto& input_touch_event_cls = *input_touch_event_res;

	if (auto err =
			input_touch_event_cls->addConstructor<const InputTouchEvent*>(
				[](ClassSelf* self, void* ctx,
				   const std::vector<ScriptingParam>& args)
					-> Result<ScriptingParam> {
					auto event = static_cast<const InputTouchEvent*>(
						std::get<const void*>(args.at(0)));
					self->setField("type", (int)event->type);
					self->setField("touchX", (int)event->touchX);
					self->setField("touchY", (int)event->touchY);
					self->setField("id", (int)event->id);
					return ScriptingParam();
				},
				nullptr)) {
		return err;
	}

	auto input_controller_event_res =
		createClass("InputControllerEvent", false);
	if (!input_controller_event_res) {
		return std::move(input_controller_event_res.error());
	}
	auto& input_controller_event_cls = *input_controller_event_res;

	if (auto err = input_controller_event_cls
					   ->addConstructor<const InputControllerEvent*>(
						   [](ClassSelf* self, void* ctx,
							  const std::vector<ScriptingParam>& args)
							   -> Result<ScriptingParam> {
							   auto event =
								   static_cast<const InputControllerEvent*>(
									   std::get<const void*>(args.at(0)));
							   self->setField("type", (int)event->type);
							   self->setField("id", (int)event->button);
							   return ScriptingParam();
						   },
						   nullptr)) {
		return err;
	}

	// Input enums
	auto input_cls = growl_class_result.get()->addClass("Input");
	if (!input_cls) {
		return std::move(input_cls.error());
	}

	if (auto err = input_cls.get()->addEnum(
			"ControllerEventType", std::vector<std::string>{
									   "Unknown",
									   "ButtonDown",
									   "ButtonUp",
									   "AxisMoved",
								   })) {
		return err;
	}

	if (auto err = input_cls.get()->addEnum(
			"ControllerButton",
			std::vector<std::string>{
				"Unknown", "A",		   "B",		   "X",			"Y",
				"DpadUp",  "DpadDown", "DpadLeft", "DpadRight", "LT",
				"RT",	   "LB",	   "RB",	   "Start",		"Select",
				"Home",	   "Misc",	   "Touchpad", "LeftStick", "RightStick",
			})) {
		return err;
	}

	if (auto err = input_cls.get()->addEnum(
			"MouseButton",
			std::vector<std::string>{
				"Unknown", "LeftClick", "RightClick", "MiddleClick"})) {
		return err;
	}

	if (auto err = input_cls.get()->addEnum(
			"PointerEventType",
			std::vector<std::string>{"Unknown", "Up", "Down", "Move"})) {
		return err;
	}

	if (auto err = input_cls.get()->addEnum(
			"KeyEventType",
			std::vector<std::string>{"Unknown", "Up", "Down"})) {
		return err;
	}

	if (auto err = input_cls.get()->addEnum(
			"Key",
			std::vector<std::string>{
				"Unknown",

				"ArrowUp",	   "ArrowDown",	  "ArrowLeft",	 "ArrowRight",

				"Number0",	   "Number1",	  "Number2",	 "Number3",
				"Number4",	   "Number5",	  "Number6",	 "Number7",
				"Number8",	   "Number9",

				"LetterA",	   "LetterB",	  "LetterC",	 "LetterD",
				"LetterE",	   "LetterF",	  "LetterG",	 "LetterH",
				"LetterI",	   "LetterJ",	  "LetterK",	 "LetterL",
				"LetterM",	   "LetterN",	  "LetterO",	 "LetterP",
				"LetterQ",	   "LetterR",	  "LetterS",	 "LetterT",
				"LetterU",	   "LetterV",	  "LetterW",	 "LetterX",
				"LetterY",	   "LetterZ",

				"FunctionF1",  "FunctionF2",  "FunctionF3",	 "FunctionF4",
				"FunctionF5",  "FunctionF6",  "FunctionF7",	 "FunctionF8",
				"FunctionF9",  "FunctionF10", "FunctionF11", "FunctionF12",
				"FunctionF13", "FunctionF14", "FunctionF15", "FunctionF16",
				"FunctionF17", "FunctionF18", "FunctionF19", "FunctionF20",
				"FunctionF21", "FunctionF22", "FunctionF23", "FunctionF24",
			})) {
		return err;
	}

	return nullptr;
}
