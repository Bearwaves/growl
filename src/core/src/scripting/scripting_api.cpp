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
	auto growl_class_result = createClass("Growl", true);
	if (growl_class_result.hasError()) {
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

	return nullptr;
}
