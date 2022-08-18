#include "growl/platforms/desktop/game_adapter.h"
#include "growl/core/graphics/window.h"
#include "growl/core/log.h"
#ifdef GROWL_IMGUI
#include "imgui.h"
#endif
#include <chrono>
#include <iostream>

using Growl::API;
using Growl::GameAdapter;

void initSDL2Plugin(API& api);
void initSoLoudPlugin(API& api);
void initMetalPlugin(API& api);
void initOpenGLPlugin(API& api);

GameAdapter::GameAdapter(std::unique_ptr<Game> game, WindowConfig window_config)
	: m_api(std::make_unique<API>())
	, m_game(std::move(game))
	, m_window_config(std::move(window_config)) {

#ifdef GROWL_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
#endif

	initSDL2Plugin(*m_api);
	initSoLoudPlugin(*m_api);
#ifdef GROWL_METAL
	initMetalPlugin(*m_api);
#else
	initOpenGLPlugin(*m_api);
#endif
	m_game->m_api = m_api.get();

	if (auto err = m_api->systemInternal->init(); err) {
		std::cout << "Failed to init system API: " << err.get()->message()
				  << std::endl;
		exit(1);
	}
	if (auto err = m_api->graphicsInternal->init(); err) {
		m_api->system().log(
			LogLevel::FATAL, "GameAdapter", "Failed to init graphics API: {}",
			err.get()->message());
		exit(2);
	}
	if (auto err = m_api->audioInternal->init(); err) {
		m_api->system().log(
			LogLevel::FATAL, "GameAdapter", "Failed to init audio API: {}",
			err.get()->message());
		exit(3);
	}
	m_api->system().log("GameAdapter", "Desktop adapter created");
}

GameAdapter::~GameAdapter() {
	if (auto err = m_game->dispose(); err) {
		m_api->system().log(
			LogLevel::FATAL, "GameAdapter", "Failed to dispose game: {}",
			err.get()->message());
		exit(4);
	}
	m_api->system().log("GameAdapter", "Desktop adapter destroying");
	m_api->graphicsInternal->dispose();
	m_api->audioInternal->dispose();
	m_api->systemInternal->dispose();
#ifdef GROWL_IMGUI
	ImGui::DestroyContext();
#endif
}

void GameAdapter::run() {
	if (auto err = m_api->graphicsInternal->setWindow(m_window_config); err) {
		m_api->system().log(
			LogLevel::FATAL, "GameAdapter", "Failed to create window: {}",
			err.get()->message());
		return;
	}
	if (auto err = m_game->init(); err) {
		m_api->system().log(
			LogLevel::FATAL, "GameAdapter", "Failed to init game: {}",
			err.get()->message());
		return;
	}
	m_api->system().log("GameAdapter", "Run!");
	while (m_api->systemInternal->isRunning()) {
		m_api->system().tick();
		m_api->graphicsInternal->begin();
		m_game->render();
		m_api->graphicsInternal->end();
	}
}
