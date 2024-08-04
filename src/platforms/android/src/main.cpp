#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/game/game.h"
#include "growl/core/graphics/window.h"
#include "growl/core/log.h"
#include "growl/scene/scene.h"
#include <android/asset_manager.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android_native_app_glue.h>
#include <memory>

using Growl::API;
using Growl::AudioAPIInternal;
using Growl::FrameTimer;
using Growl::GraphicsAPIInternal;
using Growl::LogLevel;
using Growl::ScriptingAPIInternal;
using Growl::SystemAPIInternal;
using Growl::WindowConfig;

void initAndroidPlugin(API& api, android_app* state);
void initSoLoudPlugin(API& api);
void initOpenGLPlugin(API& api);
void initLuaPlugin(API& api);
std::unique_ptr<Growl::Game> createGame();

void enableImmersiveMode(struct android_app* state) {
	JNIEnv* env{};
	state->activity->vm->AttachCurrentThread(&env, NULL);

	jclass activity_class = env->FindClass("android/app/NativeActivity");
	jmethodID get_window = env->GetMethodID(
		activity_class, "getWindow", "()Landroid/view/Window;");

	jclass window_class = env->FindClass("android/view/Window");
	jmethodID get_decor_view =
		env->GetMethodID(window_class, "getDecorView", "()Landroid/view/View;");

	jclass view_class = env->FindClass("android/view/View");
	jmethodID set_system_ui_visibility =
		env->GetMethodID(view_class, "setSystemUiVisibility", "(I)V");

	jobject window = env->CallObjectMethod(state->activity->clazz, get_window);

	jobject decor_view = env->CallObjectMethod(window, get_decor_view);

	jfieldID flag_fullscreen_id =
		env->GetStaticFieldID(view_class, "SYSTEM_UI_FLAG_FULLSCREEN", "I");
	jfieldID flag_hide_navigation_id = env->GetStaticFieldID(
		view_class, "SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I");
	jfieldID flag_immersive_sticky_id = env->GetStaticFieldID(
		view_class, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I");

	const int flag_fullscreen =
		env->GetStaticIntField(view_class, flag_fullscreen_id);
	const int flag_hide_navigation =
		env->GetStaticIntField(view_class, flag_hide_navigation_id);
	const int flag_immersive_sticky =
		env->GetStaticIntField(view_class, flag_immersive_sticky_id);
	const int flag =
		flag_fullscreen | flag_hide_navigation | flag_immersive_sticky;

	env->CallVoidMethod(decor_view, set_system_ui_visibility, flag);

	state->activity->vm->DetachCurrentThread();
}

// NOLINTNEXTLINE defined name for Android native entry point.
void android_main(struct android_app* state) {
	enableImmersiveMode(state);

	auto api = std::make_unique<API>();
	api->setFrameTimer(std::make_unique<FrameTimer>());

	initAndroidPlugin(*api, state);
	initOpenGLPlugin(*api);
	initSoLoudPlugin(*api);
	initLuaPlugin(*api);

	if (auto err = static_cast<SystemAPIInternal&>(api->system()).init()) {
		__android_log_print(
			ANDROID_LOG_FATAL, "android_main", "Failed to init system API: %s",
			err.get()->message().c_str());
		exit(1);
	}

	if (auto err = static_cast<GraphicsAPIInternal&>(api->graphics()).init();
		err) {
		api->system().log(
			LogLevel::Fatal, "android_main", "Failed to init graphics API: {}",
			err.get()->message());
		exit(2);
	}
	if (auto err = static_cast<AudioAPIInternal&>(api->audio()).init()) {
		api->system().log(
			LogLevel::Fatal, "android_main", "Failed to init audio API: {}",
			err.get()->message());
		exit(3);
	}
	if (auto err = static_cast<ScriptingAPIInternal&>(api->scripting()).init();
		err) {
		api->system().log(
			LogLevel::Fatal, "android_main", "Failed to init scripting API: {}",
			err.get()->message());
		exit(4);
	}

	if (auto err = Growl::initSceneGraph(*api)) {
		api->system().log(
			LogLevel::Fatal, "android_main", "Failed to init scene graph: {}",
			err.get()->message());
		exit(5);
	}

	api->system().log("android_main", "Android adapter created");

	auto game = createGame();
	game->setAPI(api.get());

	api->system().log("android_main", "Game created");

	// Wait for a window; process events.
	while (!state->window) {
		api->system().tick();
	}

	if (auto err = static_cast<GraphicsAPIInternal&>(api->graphics())
					   .setWindow(WindowConfig{"", 0, 0, false});
		err) {
		api->system().log(
			LogLevel::Fatal, "android_main", "Failed to create window: {}",
			err.get()->message());
		exit(4);
	}

	api->system().log("android_main", "Window created");

	if (auto err = game->init()) {
		api->system().log(
			LogLevel::Fatal, "android_main", "Failed to init game: {}",
			err.get()->message());
		exit(5);
	}

	int resize_width, resize_height;
	api->system().log("android_main", "Run!");
	while (static_cast<SystemAPIInternal&>(api->system()).isRunning()) {
		double delta_time = api->frameTimer().frame();
		api->system().tick();
		if (api->system().didResize(&resize_width, &resize_height)) {
			game->resize(resize_width, resize_height);
		}
		game->tick(delta_time);
		static_cast<GraphicsAPIInternal&>(api->graphics()).begin();
		game->render(delta_time);
		static_cast<GraphicsAPIInternal&>(api->graphics()).end();
	}

	if (auto err = game->dispose()) {
		api->system().log(
			LogLevel::Fatal, "android_main", "Failed to dispose game: {}",
			err.get()->message());
		exit(6);
	}
	game.reset();
	api->system().log("android_main", "Android adapter destroying");
	static_cast<ScriptingAPIInternal&>(api->scripting()).dispose();
	static_cast<AudioAPIInternal&>(api->audio()).dispose();
	static_cast<GraphicsAPIInternal&>(api->graphics()).dispose();
	static_cast<SystemAPIInternal&>(api->system()).dispose();
}
