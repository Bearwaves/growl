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

	Result<std::unique_ptr<Script>>
	createScript(std::string&& source, ScriptingSignature signature) override;

	Result<std::any> execute(Script& script) override;

	Result<std::unique_ptr<Class>>
	createClass(std::string&& name, bool is_static) override;

	Error setField(
		Object& obj, const std::string& name, std::any value,
		ScriptingType type) override;

	Error setClass(Object& obj, const std::string& class_name) override;

private:
	Error addConstructorToClass(
		Class* cls, const ScriptingSignature& signature, ScriptingFn fn,
		void* context) override;
	Error
	addDestructorToClass(Class* cls, ScriptingFn fn, void* context) override;
	Error addMethodToClass(
		Class* cls, const std::string& method_name,
		const ScriptingSignature& signature, ScriptingFn fn,
		void* context) override;
	Result<std::any> executeMethod(
		Object& obj, const std::string& method_name, std::vector<std::any> args,
		ScriptingSignature signature) override;

	API& api;
	lua_State* state;
};

} // namespace Growl
