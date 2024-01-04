#pragma once

#include "growl/core/error.h"
#include "growl/core/scripting/script.h"
#include <memory>

namespace Growl {

class Class;

class ScriptingAPI {
public:
	friend class Class;

	virtual ~ScriptingAPI() {}

	virtual Result<std::unique_ptr<Script>>
	createScript(std::string&& source) = 0;

	virtual Error execute(Script& script) = 0;

	virtual Result<std::unique_ptr<Class>> createClass(std::string&& name) = 0;

	template <typename T, typename Context, typename... Args>
	using ScriptingFn = T (*)(Context*, Args...);

private:
	template <typename T, typename Context, typename... Args>
	Error addMethodToClass(
		const std::string& class_name, const std::string& method_name,
		ScriptingFn<T, Context, Args...> fn, Context* context);
};

} // namespace Growl
