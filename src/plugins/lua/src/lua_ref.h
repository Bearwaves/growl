#pragma once

#include "growl/core/scripting/ref.h"

namespace Growl {

class LuaRef : public ScriptingRef {
public:
	explicit LuaRef(int ref)
		: ref{ref} {}

	int getRef() {
		return ref;
	}

private:
	int ref;
};

} // namespace Growl
