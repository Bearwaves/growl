#pragma once

#include "growl/core/error.h"
#include "growl/core/scripting/script.h"
#include <memory>

namespace Growl {

class ScriptingAPI {
public:
	virtual ~ScriptingAPI() {}

	virtual Result<std::unique_ptr<Script>>
	createScript(std::string&& source) = 0;

	virtual Error execute(Script& script) = 0;
};

} // namespace Growl
