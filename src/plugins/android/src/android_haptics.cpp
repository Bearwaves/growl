#include "android_haptics.h"
#include "android_error.h"
#include "growl/core/haptics.h"
#include <vector>

using Growl::AndroidError;
using Growl::AndroidHaptics;
using Growl::Error;
using Growl::HapticsEvent;
using Growl::HapticsEventType;

AndroidHaptics::AndroidHaptics(
	android_app* app, Paddleboat_Controller_Info* controller_info)
	: app{app} {
	if (controller_info) {
		this->rumble = controller_info->controllerFlags &
					   PADDLEBOAT_CONTROLLER_FLAG_VIBRATION_DUAL_MOTOR;
	}
}

bool AndroidHaptics::supportsEventType(HapticsEventType type) {
	switch (type) {
	case HapticsEventType::Rumble:
		return rumble;
	case HapticsEventType::TriggerRumble:
		return false;
	case HapticsEventType::Pattern:
		return pattern;
	default:
		break;
	}
	return false;
}

Error AndroidHaptics::playEvent(HapticsEvent event) {
	if (!supportsEventType(event.type)) {
		return nullptr;
	}
	Paddleboat_Vibration_Data data;
	data.durationLeft = event.duration * 1000;
	data.durationRight = event.duration * 1000;
	data.intensityLeft = event.intensity[0];
	data.intensityRight = event.intensity[1];

	JNIEnv* env;
	app->activity->vm->AttachCurrentThread(&env, nullptr);
	if (auto err = Paddleboat_setControllerVibrationData(0, &data, env)) {
		return Error(
			std::make_unique<AndroidError>(
				"Failed to set controller rumble; error code " +
				std::to_string(err)));
	}

	return nullptr;
}

Error AndroidHaptics::playPattern(std::vector<HapticsEvent> event) {
	return nullptr;
}
