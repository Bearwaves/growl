#pragma once

#include "growl/core/api/api_internal.h"
#include "growl/core/error.h"

namespace Growl {

class LuaScriptingAPI : public ScriptingAPIInternal {
public:
	explicit LuaScriptingAPI() {}

	Error init() override;
	void dispose() override;
};

} // namespace Growl
