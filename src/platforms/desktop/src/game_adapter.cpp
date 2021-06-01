#include <growl/platforms/desktop/game_adapter.h>
#include <growl/plugins/dummy/dummy_graphics.h>
#include <growl/plugins/dummy/dummy_system.h>
#include <iostream>

using Growl::GameAdapter;

GameAdapter::GameAdapter(std::unique_ptr<Game> game)
	: m_api(std::make_unique<API>()), m_game(std::move(game)) {
	std::cout << "Desktop adapter created" << std::endl;

	m_api->systemInternal = std::make_unique<DummySystemAPI>();
	m_api->graphicsInternal = std::make_unique<DummyGraphicsAPI>();
	m_game->m_api = m_api.get();

	m_api->systemInternal->init();
	m_api->graphicsInternal->init();
	m_game->init();
}

GameAdapter::~GameAdapter() {
	std::cout << "Adapter destroyed" << std::endl;
}

void GameAdapter::run() {
	std::cout << "Run!" << std::endl;
	while (m_api->systemInternal->isRunning()) {
		m_game->render();
	}
}
