#include "growl/platforms/web/game_adapter.h"
#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/frame_timer.h"
#include "growl/core/game/game.h"
#include "growl/core/graphics/window.h"
#include "growl/core/log.h"
#include "growl/scene/scene.h"
#include <chrono>
#include <emscripten.h>
#include <iostream>

using Growl::API;
using Growl::Config;
using Growl::Game;
using Growl::GameAdapter;

std::unique_ptr<API> g_api;
std::unique_ptr<Game> g_game;

GameAdapter::GameAdapter(std::unique_ptr<Game> game) {
	g_api = std::make_unique<API>();
	g_game = std::move(game);

	g_api->setSystemAPI(createSystemAPI(*g_api));
	g_api->setGraphicsAPI(createGraphicsAPI(*g_api));
	g_api->setAudioAPI(createAudioAPI(*g_api));
	g_api->setScriptingAPI(createScriptingAPI(*g_api));
	g_api->setNetworkAPI(createNetworkAPI(*g_api));

	g_game->setAPI(g_api.get());

	if (auto err = static_cast<SystemAPIInternal&>(g_api->system())
					   .init(g_game->getConfig());
		err) {
		std::cout << "Failed to init system API: " << err.get()->message()
				  << std::endl;
		exit(1);
	}
	if (auto err = static_cast<GraphicsAPIInternal&>(g_api->graphics())
					   .init(g_game->getConfig());
		err) {
		g_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init graphics API: {}",
			err.get()->message());
		exit(2);
	}
	if (auto err = static_cast<AudioAPIInternal&>(g_api->audio())
					   .init(g_game->getConfig())) {
		g_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init audio API: {}",
			err.get()->message());
		exit(3);
	}
	if (auto err = static_cast<ScriptingAPIInternal&>(g_api->scripting())
					   .init(g_game->getConfig());
		err) {
		g_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init scripting API: {}",
			err.get()->message());
		exit(4);
	}

	if (auto err = static_cast<NetworkAPIInternal&>(g_api->network())
					   .init(g_game->getConfig());
		err) {
		g_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init network API: {}",
			err.get()->message());
		exit(5);
	}

	if (auto err = Growl::initSceneGraph(*g_api)) {
		g_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init scene graph: {}",
			err.get()->message());
		exit(6);
	}
	g_api->setFrameTimer(std::make_unique<FrameTimer>());

	g_api->system().log("GameAdapter", "Web adapter created");
}

GameAdapter::~GameAdapter() {
	if (auto err = g_game->dispose()) {
		g_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to dispose game: {}",
			err.get()->message());
		exit(4);
	}
	g_game.reset();
	g_api->system().log("GameAdapter", "Web adapter destroying");
	static_cast<NetworkAPIInternal&>(g_api->network()).dispose();
	static_cast<ScriptingAPIInternal&>(g_api->scripting()).dispose();
	static_cast<AudioAPIInternal&>(g_api->audio()).dispose();
	static_cast<GraphicsAPIInternal&>(g_api->graphics()).dispose();
	static_cast<SystemAPIInternal&>(g_api->system()).dispose();
}

void GameAdapter::doLoopIteration() {
	double delta_time = g_api->frameTimer().frame();
	g_api->system().tick();
	g_game->tick(delta_time);
	static_cast<GraphicsAPIInternal&>(g_api->graphics()).begin();
	g_game->render(delta_time);
	static_cast<GraphicsAPIInternal&>(g_api->graphics()).end();
}

void GameAdapter::run() {
	if (auto err = static_cast<GraphicsAPIInternal&>(g_api->graphics())
					   .setWindow(g_game->getConfig());
		err) {
		g_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to create window: {}",
			err.get()->message());
		return;
	}
	if (auto err = g_game->init()) {
		g_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init game: {}",
			err.get()->message());
		return;
	}
	g_api->system().log("GameAdapter", "Run!");
	emscripten_set_main_loop(doLoopIteration, 0, true);
}
