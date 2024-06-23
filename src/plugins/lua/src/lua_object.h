#pragma once

#include "growl/core/scripting/object.h"
#include "lauxlib.h"
#include "lua.h"

namespace Growl {

class LuaObject : public ScriptingObject {
public:
	explicit LuaObject(lua_State* state, int ref)
		: state{state}
		, ref{ref} {}
	~LuaObject() {
		luaL_unref(state, LUA_REGISTRYINDEX, ref);
	}

	int getRef() {
		return ref;
	}

private:
	lua_State* state;
	int ref;
};

} // namespace Growl
