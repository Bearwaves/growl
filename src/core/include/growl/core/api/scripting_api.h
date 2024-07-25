#pragma once

#include "growl/core/error.h"
#include "growl/core/scripting/ref.h"
#include <memory>
#include <variant>
#include <vector>

#define GROWL_SCRIPT_VAR(type, name, var) \
	type get##name##Raw() { \
		return var; \
	} \
\
	type get##name() { \
		if (bound_script_obj) { \
			std::vector<ScriptingParam> v; \
			auto res = api->scripting().executeMethod<type>( \
				bound_script_obj.get(), "get" #name, v); \
			if (!res) { \
				api->system().log( \
					LogLevel::Warn, "Script::get" #name, "{}", \
					res.error()->message()); \
				return get##name##Raw(); \
			} \
			return std::get<type>(*res); \
		} \
		return get##name##Raw(); \
	} \
\
	void set##name##Raw(type var) { \
		this->var = var; \
	} \
\
	void set##name(type var) { \
		if (bound_script_obj) { \
			std::vector<ScriptingParam> v; \
			v.push_back(var); \
			if (auto res = api->scripting().executeMethod<void, type>( \
					bound_script_obj.get(), "set" #name, v); \
				!res) { \
				api->system().log( \
					LogLevel::Warn, "Script::set" #name, "{}", \
					res.error()->message()); \
			} \
		} else { \
			set##name##Raw(var); \
		} \
	}

namespace Growl {
class API;
class Class;
class ClassSelf;
class Script;

using ScriptingParam = std::variant<
	std::monostate,				  // Void
	bool,						  // Bool
	float,						  // Float
	int,						  // Int
	std::string_view,			  // String
	const void*,				  // Ptr
	std::unique_ptr<ScriptingRef> // Ref
	>;

using ScriptingFn = Result<ScriptingParam> (*)(
	ClassSelf*, void*, const std::vector<ScriptingParam>&);

enum class ScriptingType {
	Void,
	Bool,
	Float,
	Int,
	String,
	// A Ptr is a pointer to something outside the scripting language's state.
	Ptr,
	// A Ref is a reference to something in the scripting language's state.
	// It might be reference counted, GC'd, or something else, depending on
	// implementation.
	Ref,
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
struct ScriptingTypeOfType<bool> {
	static ScriptingType value() {
		return ScriptingType::Bool;
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
struct ScriptingTypeOfType<std::unique_ptr<ScriptingRef>> {
	static ScriptingType value() {
		return ScriptingType::Ref;
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

	Error mountGrowlScripts(API& api);

	template <typename T, typename... Args>
	Result<std::unique_ptr<Script>> createScript(std::string&& source) {
		auto signature = GetFunctionSignature<T(Args...)>::value();
		return createScript(std::move(source), signature);
	}

	virtual Result<ScriptingParam> execute(Script& script) = 0;

	virtual Result<std::unique_ptr<Class>>
	createClass(std::string&& name, bool is_static) = 0;

	virtual Error setField(
		ScriptingRef* obj, const std::string& name, ScriptingParam value) = 0;

	virtual Error
	setClass(ScriptingRef* obj, const std::string& class_name) = 0;

	template <typename... Args>
	Result<std::unique_ptr<ScriptingRef>> executeConstructor(
		const std::string& class_name, std::vector<ScriptingParam>& args) {
		auto signature = GetFunctionSignature<std::unique_ptr<ScriptingRef>(
			Args...)>::value();
		return executeConstructor(class_name, args, signature);
	}

	template <typename T, typename... Args>
	Result<ScriptingParam> executeMethod(
		ScriptingRef* obj, const std::string& method_name,
		std::vector<ScriptingParam>& args) {
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
	virtual Result<std::unique_ptr<ScriptingRef>> executeConstructor(
		const std::string& class_name, std::vector<ScriptingParam>& args,
		ScriptingSignature signature) = 0;
	virtual Result<ScriptingParam> executeMethod(
		ScriptingRef* obj, const std::string& method_name,
		std::vector<ScriptingParam>& args, ScriptingSignature signature) = 0;
};

} // namespace Growl
