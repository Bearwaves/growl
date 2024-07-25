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

	Result<ScriptingParam> execute(Script& script) override;

	Result<std::unique_ptr<Class>>
	createClass(std::string&& name, bool is_static) override;

	Error setField(
		ScriptingRef* obj, const std::string& name,
		ScriptingParam value) override;

	Error setClass(ScriptingRef* obj, const std::string& class_name) override;

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
	Result<std::unique_ptr<ScriptingRef>> executeConstructor(
		const std::string& class_name, std::vector<ScriptingParam>& args,
		ScriptingSignature signature) override;
	Result<ScriptingParam> executeMethod(
		ScriptingRef* obj, const std::string& method_name,
		std::vector<ScriptingParam>& args,
		ScriptingSignature signature) override;

	API& api;
	lua_State* state;
};

} // namespace Growl
