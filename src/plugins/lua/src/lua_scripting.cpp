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
#include <memory>
#include <string>
#include <tuple>

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
	growl_class_result.get()->addMethod(
		"log",
		static_cast<ScriptingFn<void, SystemAPI, std::string, std::string>>(
			[](SystemAPI* system, std::string tag, std::string msg) -> void {
				system->log("lua::" + tag, msg);
			}),
		&(api.system()));

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

template <typename>
struct parse_arg;

template <>
struct parse_arg<int> {
	static int value(lua_State* state, int index) {
		return lua_tointeger(state, index);
	}
};

template <>
struct parse_arg<std::string> {
	static std::string value(lua_State* state, int index) {
		return lua_tostring(state, index);
	}
};

template <typename... Args, std::size_t... Indices>
std::tuple<Args...> argsFromLuaStackHelper(
	lua_State* state, std::index_sequence<Indices...> indices) {
	return std::make_tuple(parse_arg<Args>::value(state, Indices + 1)...);
}

template <typename... Args>
std::tuple<Args...> argsFromLuaStack(lua_State* state) {
	return argsFromLuaStackHelper<Args...>(
		state, std::make_index_sequence<sizeof...(Args)>());
}

template <typename T, typename Context, typename... Args>
Error ScriptingAPI::addMethodToClass(
	const std::string& class_name, const std::string& method_name,
	ScriptingFn<T, Context, Args...> fn, Context* context) {
	auto lua = reinterpret_cast<LuaScriptingAPI*>(this);
	lua_getglobal(lua->state, class_name.c_str()); // TODO err
	lua_pushstring(lua->state, method_name.c_str());
	lua_pushlightuserdata(lua->state, reinterpret_cast<void*>(fn));
	lua_pushlightuserdata(lua->state, context);
	lua_pushcclosure(
		lua->state,
		[](lua_State* state) -> int {
			auto fn = reinterpret_cast<ScriptingFn<T, Context, Args...>>(
				const_cast<void*>(lua_topointer(state, lua_upvalueindex(1))));
			Context* ctx = reinterpret_cast<Context*>(
				const_cast<void*>(lua_topointer(state, lua_upvalueindex(2))));
			std::tuple<Context*, Args...> t = std::tuple_cat(
				std::make_tuple(ctx), argsFromLuaStack<Args...>(state));
			std::apply(fn, t);
			return 0;
		},
		2);
	lua_settable(lua->state, -3);
	return nullptr;
}
