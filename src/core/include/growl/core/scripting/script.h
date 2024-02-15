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

	// Script is copyable.
	Script(const Script&) = default;
	Script& operator=(const Script&) = default;
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
