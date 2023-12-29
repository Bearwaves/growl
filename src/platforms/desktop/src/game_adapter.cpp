#include "growl/platforms/desktop/game_adapter.h"
#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/game/game.h"
#include "growl/core/graphics/window.h"
#include "growl/core/log.h"
#include <iostream>

using Growl::API;
using Growl::Game;
using Growl::GameAdapter;

void initSDL2Plugin(API& api);
void initSoLoudPlugin(API& api);
void initMetalPlugin(API& api);
void initOpenGLPlugin(API& api);
void initLuaPlugin(API& api);

GameAdapter::GameAdapter(std::unique_ptr<Game> game, WindowConfig window_config)
	: m_api(std::make_unique<API>())
	, m_game(std::move(game))
	, m_window_config(std::move(window_config)) {

	initSDL2Plugin(*m_api);
	initSoLoudPlugin(*m_api);
	initLuaPlugin(*m_api);
#ifdef GROWL_METAL
	initMetalPlugin(*m_api);
#else
	initOpenGLPlugin(*m_api);
#endif
	m_game->setAPI(m_api.get());

	if (auto err = static_cast<SystemAPIInternal&>(m_api->system()).init();
		err) {
		std::cout << "Failed to init system API: " << err.get()->message()
				  << std::endl;
		exit(1);
	}
	if (auto err = static_cast<GraphicsAPIInternal&>(m_api->graphics()).init();
		err) {
		m_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init graphics API: {}",
			err.get()->message());
		exit(2);
	}
	if (auto err = static_cast<AudioAPIInternal&>(m_api->audio()).init(); err) {
		m_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init audio API: {}",
			err.get()->message());
		exit(3);
	}
	if (auto err =
			static_cast<ScriptingAPIInternal&>(m_api->scripting()).init();
		err) {
		m_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init scripting API: {}",
			err.get()->message());
		exit(4);
	}
	m_api->system().log("GameAdapter", "Desktop adapter created");
}

GameAdapter::~GameAdapter() {
	if (auto err = m_game->dispose(); err) {
		m_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to dispose game: {}",
			err.get()->message());
		exit(4);
	}
	m_api->system().log("GameAdapter", "Desktop adapter destroying");
	static_cast<ScriptingAPIInternal&>(m_api->scripting()).dispose();
	static_cast<AudioAPIInternal&>(m_api->audio()).dispose();
	static_cast<GraphicsAPIInternal&>(m_api->graphics()).dispose();
	static_cast<SystemAPIInternal&>(m_api->system()).dispose();
}

void GameAdapter::run() {
	if (auto err = static_cast<GraphicsAPIInternal&>(m_api->graphics())
					   .setWindow(m_window_config);
		err) {
		m_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to create window: {}",
			err.get()->message());
		return;
	}
	if (auto err = m_game->init(); err) {
		m_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init game: {}",
			err.get()->message());
		return;
	}
	m_api->system().log("GameAdapter", "Run!");
	m_game->resize(m_window_config.getWidth(), m_window_config.getHeight());
	int resize_width, resize_height;
	while (static_cast<SystemAPIInternal&>(m_api->system()).isRunning()) {
		m_api->system().tick();
		if (m_api->system().didResize(&resize_width, &resize_height)) {
			m_game->resize(resize_width, resize_height);
		}
		static_cast<GraphicsAPIInternal&>(m_api->graphics()).begin();
		m_game->render();
		static_cast<GraphicsAPIInternal&>(m_api->graphics()).end();
	}
}
