#include "growl/platforms/desktop/game_adapter.h"
#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/api/system_api.h"
#include "growl/core/frame_timer.h"
#include "growl/core/game/game.h"
#include "growl/core/graphics/window.h"
#include "growl/core/log.h"
#include "growl/scene/scene.h"
#include <iostream>

using Growl::API;
using Growl::Game;
using Growl::GameAdapter;

GameAdapter::GameAdapter(std::unique_ptr<Game> game)
	: m_api(std::make_unique<API>())
	, m_game(std::move(game)) {

	m_api->setSystemAPI(createSystemAPI(*m_api));
	m_api->setGraphicsAPI(createGraphicsAPI(*m_api));
	m_api->setAudioAPI(createAudioAPI(*m_api));
	m_api->setScriptingAPI(createScriptingAPI(*m_api));
	m_api->setNetworkAPI(createNetworkAPI(*m_api));

	m_game->setAPI(m_api.get());
	m_api->setFrameTimer(std::make_unique<FrameTimer>());

	if (auto err = static_cast<SystemAPIInternal&>(m_api->system())
					   .init(m_game->getConfig());
		err) {
		std::cout << "Failed to init system API: " << err.get()->message()
				  << std::endl;
		exit(1);
	}
	if (auto err = static_cast<GraphicsAPIInternal&>(m_api->graphics())
					   .init(m_game->getConfig());
		err) {
		m_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init graphics API: {}",
			err.get()->message());
		exit(2);
	}
	if (auto err = static_cast<AudioAPIInternal&>(m_api->audio())
					   .init(m_game->getConfig())) {
		m_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init audio API: {}",
			err.get()->message());
		exit(3);
	}
	if (auto err = static_cast<ScriptingAPIInternal&>(m_api->scripting())
					   .init(m_game->getConfig());
		err) {
		m_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init scripting API: {}",
			err.get()->message());
		exit(4);
	}

	if (auto err = static_cast<NetworkAPIInternal&>(m_api->network())
					   .init(m_game->getConfig());
		err) {
		m_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init network API: {}",
			err.get()->message());
		exit(5);
	}

	if (auto err = Growl::initSceneGraph(*m_api)) {
		m_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init scene graph: {}",
			err.get()->message());
		exit(6);
	}

	m_api->system().log("GameAdapter", "Desktop adapter created");
}

GameAdapter::~GameAdapter() {
	if (auto err = m_game->dispose()) {
		m_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to dispose game: {}",
			err.get()->message());
		exit(4);
	}
	m_game.reset();
	m_api->system().log("GameAdapter", "Desktop adapter destroying");
	static_cast<NetworkAPIInternal&>(m_api->network()).dispose();
	static_cast<ScriptingAPIInternal&>(m_api->scripting()).dispose();
	static_cast<AudioAPIInternal&>(m_api->audio()).dispose();
	static_cast<GraphicsAPIInternal&>(m_api->graphics()).dispose();
	static_cast<SystemAPIInternal&>(m_api->system()).dispose();
}

void GameAdapter::run() {
	if (auto err = static_cast<GraphicsAPIInternal&>(m_api->graphics())
					   .setWindow(m_game->getConfig());
		err) {
		m_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to create window: {}",
			err.get()->message());
		return;
	}
	if (auto err = m_game->init()) {
		m_api->system().log(
			LogLevel::Fatal, "GameAdapter", "Failed to init game: {}",
			err.get()->message());
		return;
	}
	m_api->system().log("GameAdapter", "Run!");
	int w, h;
	auto window =
		static_cast<GraphicsAPIInternal&>(m_api->graphics()).getWindow();
	window->getSize(&w, &h);
	m_game->resize(w, h);
	int resize_width, resize_height;
	while (static_cast<SystemAPIInternal&>(m_api->system()).isRunning()) {
		double delta_time = m_api->frameTimer().frame();
		m_api->system().tick();
		if (m_api->system().didResize(&resize_width, &resize_height)) {
			m_game->resize(resize_width, resize_height);
		}
		m_game->tick(delta_time);
		static_cast<GraphicsAPIInternal&>(m_api->graphics()).begin();
		m_game->render(delta_time);
		static_cast<GraphicsAPIInternal&>(m_api->graphics()).end();
	}
}
