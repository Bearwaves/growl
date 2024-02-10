#pragma once

#include "growl/core/api/scripting_api.h"
#include "growl/core/error.h"
#include <string>

namespace Growl {

namespace {
template <typename>
struct ScriptingTypeOfType;

template <typename T>
struct ScriptingTypeOfType<const T&> {
	static ScriptingType value() {
		return ScriptingTypeOfType<T>::value();
	}
};

template <>
struct ScriptingTypeOfType<std::string_view> {
	static ScriptingType value() {
		return ScriptingType::String;
	}
};

template <>
struct ScriptingTypeOfType<int> {
	static ScriptingType value() {
		return ScriptingType::Int;
	}
};

template <>
struct ScriptingTypeOfType<float> {
	static ScriptingType value() {
		return ScriptingType::Float;
	}
};

template <typename T>
struct ScriptingTypeOfType<T*> {
	static ScriptingType value() {
		return ScriptingType::Ptr;
	}
};

template <>
struct ScriptingTypeOfType<void> {
	static ScriptingType value() {
		return ScriptingType::Void;
	}
};

template <typename>
struct GetFunctionSignature;
template <typename T, typename... Args>
struct GetFunctionSignature<T(Args...)> {
	static const ScriptingSignature& value() {
		static const ScriptingSignature v{
			ScriptingTypeOfType<T>::value(),
			std::vector<ScriptingType>{ScriptingTypeOfType<Args>::value()...}};
		return v;
	}
};
} // namespace

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
	Class(std::string&& name, ScriptingAPI* lang, bool is_static)
		: name{name}
		, lang{lang}
		, is_static{is_static} {}
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

protected:
	std::string name;
	ScriptingAPI* lang;
	bool is_static;
};

class ClassSelf {
public:
	virtual void setField(const std::string& name, void* val) = 0;
	virtual const void* getField(const std::string& name) = 0;
};

} // namespace Growl
