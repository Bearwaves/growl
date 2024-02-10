#pragma once

#include "growl/core/scripting/class.h"

struct lua_State;
namespace Growl {

class LuaSelf final : public ClassSelf {
public:
	LuaSelf(lua_State* state)
		: state{state} {}
	void setField(const std::string& name, void* val);
	const void* getField(const std::string& name);

private:
	lua_State* state;
};

} // namespace Growl
