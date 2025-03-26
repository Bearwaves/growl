#pragma once

#include "growl/core/system/preferences.h"

namespace Growl {

class IOSSystemAPI;

class IOSPreferences : public Preferences {
public:
	IOSPreferences(bool ubiquitous, bool shared, nlohmann::json&& j);
	~IOSPreferences();
	void store() override;

private:
	bool ubiquitous;
};

} // namespace Growl
