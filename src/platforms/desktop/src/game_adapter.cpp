#include <chrono>
#include <growl/core/graphics/window.h>
#include <growl/platforms/desktop/game_adapter.h>
#include <iostream>

using Growl::API;
using Growl::GameAdapter;
using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::seconds;

void initSDL2Plugin(API& api);
void initMetalPlugin(API& api);

GameAdapter::GameAdapter(std::unique_ptr<Game> game, WindowConfig windowConfig)
	: m_api(std::make_unique<API>())
	, m_game(std::move(game))
	, m_window_config(std::move(windowConfig)) {
	std::cout << "Desktop adapter created" << std::endl;

	initSDL2Plugin(*m_api);
	initMetalPlugin(*m_api);
	m_game->m_api = m_api.get();

	m_api->systemInternal->init();
	m_api->graphicsInternal->init();
}

GameAdapter::~GameAdapter() {
	m_api->graphicsInternal->dispose();
	m_api->systemInternal->dispose();
	std::cout << "Adapter destroyed" << std::endl;
}

void GameAdapter::run() {
	m_api->graphicsInternal->setWindow(m_window_config);
	m_game->init();
	std::cout << "Run!" << std::endl;
	while (m_api->systemInternal->isRunning()) {
		auto t1 = high_resolution_clock::now();
		m_api->system()->tick();
		m_api->graphicsInternal->begin();
		m_game->render();
		m_api->graphicsInternal->end();
		auto t2 = high_resolution_clock::now();
		m_api->graphicsInternal->setDeltaTime(
			duration<double, seconds::period>(t2 - t1).count());
	}
}
