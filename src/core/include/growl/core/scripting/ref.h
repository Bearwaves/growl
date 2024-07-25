#pragma once

#include <memory>

namespace Growl {

// A ScriptingRef is a reference to something that exists in the scripting
// language's execution state or VM, which we can't normally access from C.
class ScriptingRef {
public:
	ScriptingRef() = default;
	virtual ~ScriptingRef() = default;
	virtual std::unique_ptr<ScriptingRef> copy() = 0;

	// Move-only; copying is implementation-dependent.
	ScriptingRef(const ScriptingRef&) = delete;
	ScriptingRef& operator=(const ScriptingRef&) = delete;
	ScriptingRef(ScriptingRef&&) = default;
	ScriptingRef& operator=(ScriptingRef&&) = default;
};

} // namespace Growl
