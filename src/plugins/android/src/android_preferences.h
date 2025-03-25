#pragma once
#include "growl/core/system/preferences.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace Growl {

class AndroidPreferences : public Preferences {
public:
	AndroidPreferences(android_app* app, nlohmann::json&& j);
	~AndroidPreferences();
	void store() override;
	static std::string getPreferencesJSON(android_app* app);

private:
	android_app* app;
	static void
	setPreferencesJSON(android_app* app, std::string preferences_json);
};

} // namespace Growl