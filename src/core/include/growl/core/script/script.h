#pragma once

#include <string>

namespace Growl {

class Script {
public:
	explicit Script(std::string&& source)
		: source{std::move(source)} {}
	virtual ~Script() = default;

	// Script is move-only
	Script(const Script&) = delete;
	Script& operator=(const Script&) = delete;
	Script(Script&&) = default;
	Script& operator=(Script&&) = default;

	std::string& getSource() {
		return this->source;
	}

protected:
	std::string source;
};

} // namespace Growl
