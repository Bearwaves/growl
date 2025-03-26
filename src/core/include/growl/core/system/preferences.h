#pragma once

#include "nlohmann/json.hpp"

namespace Growl {

class Preferences {
public:
	Preferences(bool shared, nlohmann::json&& j)
		: shared{shared}
		, j{std::move(j)} {}
	virtual ~Preferences() = default;
	virtual void store() = 0;

	nlohmann::json& data() {
		return j;
	}

	bool isShared() {
		return shared;
	}

private:
	bool shared;
	nlohmann::json j;
};

} // namespace Growl
