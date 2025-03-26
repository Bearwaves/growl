#pragma once
#include "growl/core/system/preferences.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace Growl {

class AndroidPreferences : public Preferences {
public:
	AndroidPreferences(android_app* app, bool shared, nlohmann::json&& j);
	~AndroidPreferences();
	void store() override;
	static std::string getPreferencesJSON(android_app* app, bool shared);

private:
	android_app* app;
	static void setPreferencesJSON(
		android_app* app, bool shared, std::string preferences_json);
};

} // namespace Growl