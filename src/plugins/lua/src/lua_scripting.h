#pragma once

#include "growl/core/api/api_internal.h"
#include "growl/core/error.h"
#include "growl/core/script/script.h"
#include "lua.h"
#include <memory>

namespace Growl {

class LuaScriptingAPI : public ScriptingAPIInternal {
public:
	explicit LuaScriptingAPI(SystemAPI& system)
		: system{system} {}

	Error init() override;
	void dispose() override;

	Result<std::unique_ptr<Script>> createScript(std::string&& source) override;

	Error execute(Script& script) override;

private:
	SystemAPI& system;
	lua_State* state;

	Error bindLambda(
		const char* name, lua_CFunction fn,
		std::vector<void*> captures = std::vector<void*>());
};

} // namespace Growl
