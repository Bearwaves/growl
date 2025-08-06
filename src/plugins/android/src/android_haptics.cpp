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
	} else {
		this->pattern = hasSystemHaptics();
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

Error AndroidHaptics::playEvent(HapticsEvent event, float intensity) {
	if (!supportsEventType(event.type)) {
		return nullptr;
	}
	Paddleboat_Vibration_Data data;
	data.durationLeft = event.duration * 1000;
	data.durationRight = event.duration * 1000;
	data.intensityLeft = event.intensity[0] * intensity;
	data.intensityRight = event.intensity[1] * intensity;

	if (auto err =
			Paddleboat_setControllerVibrationData(0, &data, getJNIEnv())) {
		return Error(
			std::make_unique<AndroidError>(
				"Failed to set controller rumble; error code " +
				std::to_string(err)));
	}

	return nullptr;
}

Error AndroidHaptics::playPattern(
	std::vector<HapticsEvent> haptic_pattern, float intensity) {
	auto env = getJNIEnv();

	jclass activity_class =
		env->GetObjectClass(app->activity->javaGameActivity);
	jmethodID get_class_loader_method = env->GetMethodID(
		activity_class, "getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject class_loader = env->CallObjectMethod(
		app->activity->javaGameActivity, get_class_loader_method);

	jclass class_loader_cls = env->FindClass("java/lang/ClassLoader");
	jmethodID load_class_method = env->GetMethodID(
		class_loader_cls, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jclass haptics_event_cls = (jclass)env->CallObjectMethod(
		class_loader, load_class_method,
		env->NewStringUTF("com.bearwaves.growl.HapticsEffect"));
	jmethodID haptics_event_ctor =
		env->GetMethodID(haptics_event_cls, "<init>", "(DFFF)V");

	jobjectArray array =
		env->NewObjectArray(haptic_pattern.size(), haptics_event_cls, nullptr);
	for (int i = 0; i < haptic_pattern.size(); i++) {
		auto& event = haptic_pattern.at(i);
		jobject haptics_event = env->NewObject(
			haptics_event_cls, haptics_event_ctor, event.duration,
			event.intensity[0] * intensity, event.intensity[1] * intensity,
			event.offset);
		env->SetObjectArrayElement(array, i, haptics_event);
	}

	jmethodID method = env->GetMethodID(
		activity_class, "playVibrationPattern",
		"([Lcom/bearwaves/growl/HapticsEffect;)V");
	env->CallVoidMethod(app->activity->javaGameActivity, method, array);

	return nullptr;
}

JNIEnv* AndroidHaptics::getJNIEnv() {
	JNIEnv* env;
	app->activity->vm->AttachCurrentThread(&env, nullptr);
	return env;
}

bool AndroidHaptics::hasSystemHaptics() {
	auto env = getJNIEnv();
	jclass activity_class =
		env->GetObjectClass(app->activity->javaGameActivity);
	jmethodID method =
		env->GetMethodID(activity_class, "supportsHaptics", "()Z");
	jboolean supports_haptics =
		env->CallBooleanMethod(app->activity->javaGameActivity, method);
	return supports_haptics == JNI_TRUE;
}