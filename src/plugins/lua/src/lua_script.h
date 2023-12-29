#pragma once

#include "growl/core/script/script.h"

namespace Growl {
class LuaScript : public Script {
public:
	LuaScript(std::string&& source)
		: Script{std::move(source)} {}
};
} // namespace Growl
