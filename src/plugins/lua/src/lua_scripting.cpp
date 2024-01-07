#include "lua_scripting.h"
#include "growl/core/api/scripting_api.h"
#include "growl/core/api/system_api.h"
#include "growl/core/error.h"
#include "growl/core/scripting/class.h"
#include "growl/core/scripting/script.h"
#include "lauxlib.h"
#include "lua.h"
#include "lua_error.h"
#include "lua_script.h"
#include <any>
#include <memory>
#include <string>
#include <string_view>

using Growl::Class;
using Growl::Error;
using Growl::LuaError;
using Growl::LuaScript;
using Growl::LuaScriptingAPI;
using Growl::Result;
using Growl::Script;
using Growl::ScriptingAPI;
using Growl::SystemAPI;

Error LuaScriptingAPI::init() {
	this->state = luaL_newstate();

	// Create Growl globals.
	auto growl_class_result = createClass("Growl");
	if (growl_class_result.hasError()) {
		return std::move(growl_class_result.error());
	}

	if (auto err =
			growl_class_result.get()
				->addMethod<void, std::string_view, std::string_view>(
					"log",
					[](void* ctx,
					   const std::vector<std::any>& args) -> Result<std::any> {
						SystemAPI* system = static_cast<SystemAPI*>(ctx);
						auto& tag =
							std::any_cast<const std::string&>(args.at(0));
						auto& msg =
							std::any_cast<const std::string&>(args.at(1));
						system->log(std::string("lua::").append(tag), msg);
						return std::any();
					},
					&(api.system()));
		err) {
		return err;
	}

	api.system().log("LuaScriptingAPI", "Created Lua state");

	return nullptr;
}

void LuaScriptingAPI::dispose() {
	api.system().log("LuaScriptingAPI", "Closing Lua state");
	lua_close(this->state);
}

Result<std::unique_ptr<Script>>
LuaScriptingAPI::createScript(std::string&& source) {
	return std::unique_ptr<Script>(
		std::make_unique<LuaScript>(std::move(source)));
}

Error LuaScriptingAPI::execute(Script& script) {
	if (luaL_loadstring(this->state, script.getSource().c_str()) ||
		lua_pcall(this->state, 0, 0, 0)) {
		lua_tostring(this->state, -1);
		auto err = std::make_unique<LuaError>(
			std::string("Failed to execute script: ") +
			std::string(lua_tostring(this->state, -1)));
		lua_pop(this->state, 1);
		return std::move(err);
	}
	return nullptr;
}

Result<std::unique_ptr<Class>>
LuaScriptingAPI::createClass(std::string&& name) {
	lua_newtable(this->state);
	lua_setglobal(this->state, name.c_str());
	return std::make_unique<Class>(std::move(name), this);
}

Error LuaScriptingAPI::addMethodToClass(
	const std::string& class_name, const std::string& method_name,
	const ScriptingSignature& signature, ScriptingFn fn, void* context) {
	lua_getglobal(this->state, class_name.c_str()); // TODO err
	lua_pushstring(this->state, method_name.c_str());
	lua_pushlightuserdata(this->state, reinterpret_cast<void*>(fn));
	lua_pushlightuserdata(this->state, context);
	ScriptingSignature* sig = static_cast<ScriptingSignature*>(
		lua_newuserdata(this->state, sizeof(signature)));
	*sig = signature;
	lua_pushcclosure(
		this->state,
		[](lua_State* state) -> int {
			auto fn = reinterpret_cast<ScriptingFn>(
				const_cast<void*>(lua_topointer(state, lua_upvalueindex(1))));
			void* ctx =
				const_cast<void*>(lua_topointer(state, lua_upvalueindex(2)));
			ScriptingSignature* signature = static_cast<ScriptingSignature*>(
				lua_touserdata(state, lua_upvalueindex(3)));
			std::vector<std::any> args(signature->arg_types.size());
			for (size_t i = 0; i < signature->arg_types.size(); i++) {
				switch (signature->arg_types[i]) {
				case ScriptingType::String:
					args[i] = std::string{lua_tostring(state, i + 1)};
					break;
				default:
					continue;
				}
			}
			fn(ctx, args);
			return 0;
		},
		3);
	lua_settable(this->state, -3);
	return nullptr;
}
