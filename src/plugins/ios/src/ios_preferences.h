#pragma once

#include "growl/core/system/preferences.h"
namespace Growl {

class IOSSystemAPI;

class IOSPreferences : public Preferences {
public:
	IOSPreferences(IOSSystemAPI& api, bool shared, nlohmann::json&& j);
	~IOSPreferences();
	void store() override;

private:
	IOSSystemAPI& api;
};

} // namespace Growl
