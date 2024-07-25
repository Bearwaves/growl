#pragma once

#include "growl/core/scripting/ref.h"
#include "lauxlib.h"
#include "lua.h"

namespace Growl {

class LuaRef : public ScriptingRef {
public:
	explicit LuaRef(lua_State* state, int ref)
		: state{state}
		, ref{ref} {}
	~LuaRef() {
		luaL_unref(state, LUA_REGISTRYINDEX, ref);
	}

	int getRef() {
		return ref;
	}

	std::unique_ptr<ScriptingRef> copy() override {
		lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
		int new_ref = luaL_ref(state, LUA_REGISTRYINDEX);
		return std::make_unique<LuaRef>(state, new_ref);
	}

private:
	lua_State* state;
	int ref;
};

} // namespace Growl
