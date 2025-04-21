#pragma once

#include "growl/core/haptics.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <paddleboat/paddleboat.h>

namespace Growl {

class AndroidHaptics : public Growl::HapticsDevice {
public:
	explicit AndroidHaptics(
		android_app* app, Paddleboat_Controller_Info* info = nullptr);

	bool supportsEventType(HapticsEventType type) override;
	Error playEvent(HapticsEvent event) override;
	Error playPattern(std::vector<HapticsEvent> pattern) override;

private:
	android_app* app;
	bool rumble = false;
	bool pattern = false;
};

} // namespace Growl