#pragma once

#include "growl/core/error.h"
#include "growl/core/scripting/object.h"
#include <any>
#include <memory>
#include <vector>

#define SCRIPTED_GETSET(type, name, var) \
	type get##name(bool from_script = false) { \
		if (bound_script_obj && !from_script) { \
			auto res = scripting_api->executeMethod<type>( \
				*bound_script_obj, "get" #name, std::vector<std::any>()); \
			if (!res) { \
				return var; \
			} \
			return std::any_cast<type>(*res); \
		} \
		return var; \
	} \
\
	void set##name(type var, bool from_script = false) { \
		if (bound_script_obj && !from_script) { \
			scripting_api->executeMethod<void, type>( \
				*bound_script_obj, "set" #name, {var}); \
		} else { \
			this->var = var; \
		} \
	}

namespace Growl {
class Class;
class ClassSelf;
class Script;

using ScriptingFn =
	Result<std::any> (*)(ClassSelf*, void*, const std::vector<std::any>&);

enum class ScriptingType {
	Void,
	Ptr,
	Int,
	String,
	Float,
	Object,
};

struct ScriptingSignature {
	ScriptingType return_type;
	std::vector<ScriptingType> arg_types;
};

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

template <>
struct ScriptingTypeOfType<Object> {
	static ScriptingType value() {
		return ScriptingType::Object;
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

class ScriptingAPI {
public:
	friend class Class;

	virtual ~ScriptingAPI() {}

	template <typename T, typename... Args>
	Result<std::unique_ptr<Script>> createScript(std::string&& source) {
		auto signature = GetFunctionSignature<T(Args...)>::value();
		return createScript(std::move(source), signature);
	}

	virtual Result<std::any> execute(Script& script) = 0;

	virtual Result<std::unique_ptr<Class>>
	createClass(std::string&& name, bool is_static) = 0;

	virtual Error setField(
		Object& obj, const std::string& name, std::any value,
		ScriptingType type) = 0;

	virtual Error setClass(Object& obj, const std::string& class_name) = 0;

	template <typename T, typename... Args>
	Result<std::any> executeMethod(
		Object& obj, const std::string& method_name,
		std::vector<std::any> args) {
		auto signature = GetFunctionSignature<T(Args...)>::value();
		return executeMethod(obj, method_name, args, signature);
	}

private:
	virtual Result<std::unique_ptr<Script>>
	createScript(std::string&& source, ScriptingSignature signature) = 0;
	virtual Error addConstructorToClass(
		Class* cls, const ScriptingSignature& signature, ScriptingFn fn,
		void* context) = 0;
	virtual Error
	addDestructorToClass(Class* cls, ScriptingFn fn, void* context) = 0;
	virtual Error addMethodToClass(
		Class* cls, const std::string& method_name,
		const ScriptingSignature& signature, ScriptingFn fn, void* context) = 0;
	virtual Result<std::any> executeMethod(
		Object& obj, const std::string& method_name, std::vector<std::any> args,
		ScriptingSignature signature) = 0;
};

} // namespace Growl
