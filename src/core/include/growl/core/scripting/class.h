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

	template <typename T, typename... Args>
	Error addMethod(const std::string& name, ScriptingFn fn, void* context) {
		auto signature = GetFunctionSignature<T(Args...)>::value();
		return lang->addMethodToClass(this->name, name, signature, fn, context);
	}

protected:
	std::string name;
	ScriptingAPI* lang;
};

} // namespace Growl
