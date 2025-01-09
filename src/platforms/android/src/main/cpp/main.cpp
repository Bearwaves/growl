#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/game/game.h"
#include "growl/core/log.h"
#include "growl/scene/scene.h"
#include <android/asset_manager.h>
#include <android/log.h>
#include <android/native_window.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <memory>

using Growl::API;
using Growl::AudioAPIInternal;
using Growl::Config;
using Growl::FrameTimer;
using Growl::GraphicsAPIInternal;
using Growl::LogLevel;
using Growl::ScriptingAPIInternal;
using Growl::SystemAPIInternal;

void initAndroidPlugin(API& api, android_app* state);
void initSoLoudPlugin(API& api);
void initOpenGLPlugin(API& api);
void initLuaPlugin(API& api);
std::unique_ptr<Growl::Game> createGame();

// NOLINTNEXTLINE defined name for Android native entry point.
void android_main(struct android_app* state) {
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
					   .setWindow(Config{"", 0, 0, false});
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
