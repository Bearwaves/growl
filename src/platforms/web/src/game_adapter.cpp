#include "growl/platforms/web/game_adapter.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/game/game.h"
#include "growl/core/graphics/window.h"
#include "growl/core/log.h"
#include <chrono>
#include <emscripten.h>
#include <iostream>

using Growl::API;
using Growl::Game;
using Growl::GameAdapter;

void initSDL2Plugin(API& api);
void initSoLoudPlugin(API& api);
void initOpenGLPlugin(API& api);
std::unique_ptr<API> g_api;
std::unique_ptr<Game> g_game;

GameAdapter::GameAdapter(std::unique_ptr<Game> game, WindowConfig window_config)
	: window_config{window_config} {
	g_api = std::make_unique<API>();
	g_game = std::move(game);
	initSDL2Plugin(*g_api);
	initSoLoudPlugin(*g_api);
	initOpenGLPlugin(*g_api);
	g_game->setAPI(g_api.get());

	if (auto err = static_cast<SystemAPIInternal&>(g_api->system()).init();
		err) {
		std::cout << "Failed to init system API: " << err.get()->message()
				  << std::endl;
		exit(1);
	}
	if (auto err = static_cast<GraphicsAPIInternal&>(g_api->graphics()).init();
		err) {
		g_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init graphics API: {}",
			err.get()->message());
		exit(2);
	}
	if (auto err = static_cast<AudioAPIInternal&>(g_api->audio()).init(); err) {
		g_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init audio API: {}",
			err.get()->message());
		exit(3);
	}
	g_api->system().log("GameAdapter", "Web adapter created");
}

GameAdapter::~GameAdapter() {
	if (auto err = g_game->dispose(); err) {
		g_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to dispose game: {}",
			err.get()->message());
		exit(4);
	}
	g_api->system().log("GameAdapter", "Web adapter destroying");
	static_cast<AudioAPIInternal&>(g_api->audio()).dispose();
	static_cast<GraphicsAPIInternal&>(g_api->graphics()).dispose();
	static_cast<SystemAPIInternal&>(g_api->system()).dispose();
}

void GameAdapter::doLoopIteration() {
	g_api->system().tick();
	static_cast<GraphicsAPIInternal&>(g_api->graphics()).begin();
	g_game->render();
	static_cast<GraphicsAPIInternal&>(g_api->graphics()).end();
}

void GameAdapter::run() {
	if (auto err = static_cast<GraphicsAPIInternal&>(g_api->graphics())
					   .setWindow(window_config);
		err) {
		g_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to create window: {}",
			err.get()->message());
		return;
	}
	if (auto err = g_game->init(); err) {
		g_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init game: {}",
			err.get()->message());
		return;
	}
	g_api->system().log("GameAdapter", "Run!");
	emscripten_set_main_loop(doLoopIteration, 0, true);
}
