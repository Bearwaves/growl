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
				nullptr);
		err) {
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
				nullptr);
		err) {
		return err;
	}

	return nullptr;
}
