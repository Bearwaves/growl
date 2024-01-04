#pragma once

#include "growl/core/api/scripting_api.h"
#include "growl/core/error.h"
#include <string>

namespace Growl {

class Class {
public:
	Class(std::string&& name, ScriptingAPI* lang)
		: name{name}
		, lang{lang} {}
	virtual ~Class() = default;

	// Class is move-only
	Class(const Class&) = delete;
	Class& operator=(const Class&) = delete;
	Class(Class&&) = default;
	Class& operator=(Class&&) = default;

	template <typename T, typename Context, typename... Args>
	Error addMethod(
		const std::string& name,
		ScriptingAPI::ScriptingFn<T, Context, Args...> fn, Context* context) {
		return lang->addMethodToClass(this->name, name, fn, context);
	}

protected:
	std::string name;
	ScriptingAPI* lang;
};

} // namespace Growl
