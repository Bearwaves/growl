#include "android_preferences.h"

using Growl::AndroidPreferences;
using Growl::Preferences;

AndroidPreferences::AndroidPreferences(
	android_app* app, bool shared, nlohmann::json&& j)
	: Preferences{shared, std::move(j)}
	, app{app} {}

AndroidPreferences::~AndroidPreferences() {
	store();
}

void AndroidPreferences::store() {
	setPreferencesJSON(app, isShared(), data().dump());
}

std::string
AndroidPreferences::getPreferencesJSON(android_app* app, bool shared) {
	JNIEnv* env{};
	app->activity->vm->AttachCurrentThread(&env, NULL);
	jclass activity_class =
		env->GetObjectClass(app->activity->javaGameActivity);
	jmethodID method = env->GetMethodID(
		activity_class, "getPreferencesJSON", "(Z)Ljava/lang/String;");
	jstring preferences_json = (jstring)env->CallObjectMethod(
		app->activity->javaGameActivity, method, shared);
	auto c_str = env->GetStringUTFChars(preferences_json, nullptr);
	std::string result{c_str};
	env->ReleaseStringUTFChars(preferences_json, c_str);
	return result;
}

void AndroidPreferences::setPreferencesJSON(
	android_app* app, bool shared, std::string preferences_json) {
	JNIEnv* env{};
	app->activity->vm->AttachCurrentThread(&env, NULL);
	jstring json_string = env->NewStringUTF(preferences_json.c_str());
	jclass activity_class =
		env->GetObjectClass(app->activity->javaGameActivity);
	jmethodID method = env->GetMethodID(
		activity_class, "setPreferencesJSON", "(ZLjava/lang/String;)V");
	env->CallVoidMethod(
		app->activity->javaGameActivity, method, shared, json_string);
}
