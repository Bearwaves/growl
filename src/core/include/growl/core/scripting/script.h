#pragma once

#include "growl/core/api/scripting_api.h"
#include <string>

namespace Growl {

class Script {
public:
	explicit Script(std::string&& source, ScriptingSignature signature)
		: source{std::move(source)}
		, signature{signature} {}
	virtual ~Script() = default;

	// Script is move-only
	Script(const Script&) = delete;
	Script& operator=(const Script&) = delete;
	Script(Script&&) = default;
	Script& operator=(Script&&) = default;

	std::string& getSource() {
		return this->source;
	}

	const ScriptingSignature& getSignature() {
		return signature;
	}

protected:
	std::string source;
	ScriptingSignature signature;
};

} // namespace Growl
