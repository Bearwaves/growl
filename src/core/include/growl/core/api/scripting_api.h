#pragma once

#include "growl/core/error.h"
#include "growl/core/scripting/script.h"
#include <any>
#include <memory>
#include <vector>

namespace Growl {

using ScriptingFn = Result<std::any> (*)(void*, const std::vector<std::any>&);

enum class ScriptingType {
	Void,
	Ptr,
	Int,
	String,
	Float,
};

struct ScriptingSignature {
	ScriptingType return_type;
	std::vector<ScriptingType> arg_types;
};

class Class;

class ScriptingAPI {
public:
	friend class Class;

	virtual ~ScriptingAPI() {}

	virtual Result<std::unique_ptr<Script>>
	createScript(std::string&& source) = 0;

	virtual Error execute(Script& script) = 0;

	virtual Result<std::unique_ptr<Class>> createClass(std::string&& name) = 0;

private:
	virtual Error addMethodToClass(
		const std::string& class_name, const std::string& method_name,
		const ScriptingSignature& signature, ScriptingFn fn, void* context) = 0;
};

} // namespace Growl
