#pragma once

#include "growl/core/scripting/object.h"
#include "lauxlib.h"
#include "lua.h"
#include <iostream>

namespace Growl {

class LuaObject : public ScriptingObject {
public:
	explicit LuaObject(lua_State* state, int ref, bool is_ref = false)
		: ScriptingObject{is_ref}
		, state{state}
		, ref{ref} {}
	~LuaObject() {
		if (!is_ref) {
			std::cout << "unref " << ref << std::endl;
			luaL_unref(state, LUA_REGISTRYINDEX, ref);
		} else {
			std::cout << "Reffy" << std::endl;
		}
	}

	int getRef() {
		return ref;
	}

private:
	lua_State* state;
	int ref;
};

} // namespace Growl
