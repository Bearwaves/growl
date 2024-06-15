#pragma once

#include "growl/core/scripting/class.h"

struct lua_State;
namespace Growl {

class LuaSelf final : public ClassSelf {
public:
	LuaSelf(lua_State* state)
		: state{state} {}
	void setField(const std::string& name, ScriptingParam val);
	const ScriptingParam getField(const std::string& name, ScriptingType type);

private:
	lua_State* state;
};

} // namespace Growl
