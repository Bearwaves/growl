#pragma once

namespace Growl {

// A ScriptingObject is a reference to something that exists in the scripting
// language's execution state or VM, which we can't normally access from C.
class ScriptingObject {
public:
	ScriptingObject() = default;
	virtual ~ScriptingObject() = default;

	// Reference is move-only
	ScriptingObject(const ScriptingObject&) = delete;
	ScriptingObject& operator=(const ScriptingObject&) = delete;
	ScriptingObject(ScriptingObject&&) = default;
	ScriptingObject& operator=(ScriptingObject&&) = default;
};

} // namespace Growl
