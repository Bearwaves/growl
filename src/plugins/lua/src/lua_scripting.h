#pragma once

#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/error.h"
#include "growl/core/scripting/script.h"
#include "lua.h"
#include <memory>

namespace Growl {

class LuaScriptingAPI : public ScriptingAPIInternal {
	friend class ScriptingAPI;

public:
	explicit LuaScriptingAPI(API& api)
		: api{api} {}

	Error init() override;
	void dispose() override;

	Result<std::unique_ptr<Script>> createScript(std::string&& source) override;

	Error execute(Script& script) override;

	Result<std::unique_ptr<Class>> createClass(std::string&& name) override;

private:
	Error addMethodToClass(
		const std::string& class_name, const std::string& method_name,
		const ScriptingSignature& signature, ScriptingFn fn,
		void* context) override;

	API& api;
	lua_State* state;
};

} // namespace Growl
