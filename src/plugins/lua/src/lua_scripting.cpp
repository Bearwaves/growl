#include "lua_scripting.h"
#include "growl/core/error.h"
#include "growl/core/script/script.h"
#include "lauxlib.h"
#include "lua.h"
#include "lua_error.h"
#include "lua_script.h"
#include <string>

using Growl::Error;
using Growl::LuaError;
using Growl::LuaScript;
using Growl::LuaScriptingAPI;
using Growl::Result;
using Growl::Script;

Error LuaScriptingAPI::init() {
	this->state = luaL_newstate();

	// Create Growl globals.
	lua_newtable(this->state);
	this->bindLambda(
		"log",
		[](lua_State* state) -> int {
			SystemAPI* sys =
				(SystemAPI*)lua_topointer(state, lua_upvalueindex(1));
			if (!sys) {
				lua_pushstring(state, "Couldn't get Growl SystemAPI binding");
				return 1;
			}
			const char* tag = lua_tostring(state, 1);
			const char* msg = lua_tostring(state, 2);
			sys->log(std::string("lua::") + tag, msg);
			return 0;
		},
		std::vector<void*>{&(this->system)});
	lua_setglobal(this->state, "Growl");

	system.log("LuaScriptingAPI", "Created Lua state");

	return nullptr;
}

void LuaScriptingAPI::dispose() {
	system.log("LuaScriptingAPI", "Closing Lua state");
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

Error LuaScriptingAPI::bindLambda(
	const char* name, lua_CFunction fn, std::vector<void*> captures) {
	lua_pushstring(this->state, name);
	for (const auto& capture : captures) {
		lua_pushlightuserdata(this->state, capture);
	}
	lua_pushcclosure(this->state, fn, captures.size());
	lua_settable(this->state, -3);
	return nullptr;
}
