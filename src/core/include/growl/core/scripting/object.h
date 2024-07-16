#pragma once

#include <memory>

namespace Growl {

class ScriptingRef;

class ScriptingObject {
public:
	ScriptingObject() = default;
	virtual ~ScriptingObject() = default;
	virtual std::unique_ptr<ScriptingRef> makeRef() = 0;

	// ScriptingObject is move-only
	ScriptingObject(const ScriptingObject&) = delete;
	ScriptingObject& operator=(const ScriptingObject&) = delete;
	ScriptingObject(ScriptingObject&&) = default;
	ScriptingObject& operator=(ScriptingObject&&) = default;
};

} // namespace Growl
