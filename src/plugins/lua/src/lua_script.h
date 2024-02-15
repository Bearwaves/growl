#pragma once

#include "growl/core/scripting/script.h"

namespace Growl {
class LuaScript : public Script {
public:
	LuaScript(std::string&& source, ScriptingSignature signature)
		: Script{std::move(source), signature} {}
};
} // namespace Growl
