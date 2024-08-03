#pragma once

#include "growl/core/api/scripting_api.h"
#include "growl/core/error.h"
#include <string>

namespace Growl {

class ScriptingError : public BaseError {
public:
	explicit ScriptingError(std::string message)
		: message_str{message} {}

	std::string message() override {
		return message_str;
	}

private:
	std::string message_str;
};

class Class {
	friend class ClassSelf;

public:
	Class(
		std::string&& name, ScriptingAPI* lang, bool is_static,
		Class* parent = nullptr)
		: name{name}
		, lang{lang}
		, is_static{is_static}
		, parent{parent} {}
	virtual ~Class() = default;

	// Class is move-only
	Class(const Class&) = delete;
	Class& operator=(const Class&) = delete;
	Class(Class&&) = default;
	Class& operator=(Class&&) = default;

	const std::string& getName() {
		return name;
	}

	const bool isStatic() {
		return is_static;
	}

	Class* getParent() {
		return parent;
	}

	template <typename... Args>
	Error addConstructor(ScriptingFn fn, void* context) {
		if (is_static) {
			return std::make_unique<ScriptingError>(
				"Cannot attach constructor to static class");
		}
		auto signature = GetFunctionSignature<void(Args...)>::value();
		return lang->addConstructorToClass(this, signature, fn, context);
	}

	Error addDestructor(ScriptingFn fn, void* context) {
		if (is_static) {
			return std::make_unique<ScriptingError>(
				"Cannot attach destructor to static class");
		}
		return lang->addDestructorToClass(this, fn, context);
	}

	template <typename T, typename... Args>
	Error addMethod(const std::string& name, ScriptingFn fn, void* context) {
		auto signature = GetFunctionSignature<T(Args...)>::value();
		return lang->addMethodToClass(this, name, signature, fn, context);
	}

	Result<std::unique_ptr<Class>> addClass(std::string&& name) {
		if (!is_static) {
			return Error(std::make_unique<ScriptingError>(
				"Cannot add class to non-static class"));
		}
		return lang->addClassToClass(this, std::move(name));
	}

	Error
	addEnum(const std::string& name, const std::vector<std::string>& values) {
		if (!is_static) {
			return Error(std::make_unique<ScriptingError>(
				"Cannot add enum to non-static class"));
		}
		return lang->addEnumToClass(this, name, values);
	}

protected:
	std::string name;
	ScriptingAPI* lang;
	bool is_static;
	Class* parent;
};

class ClassSelf {
public:
	virtual void setField(const std::string& name, ScriptingParam val) = 0;
	virtual const ScriptingParam
	getField(const std::string& name, ScriptingType type) = 0;
};

} // namespace Growl
