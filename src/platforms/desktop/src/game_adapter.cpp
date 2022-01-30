#include <growl/core/graphics/window.h>
#include <growl/platforms/desktop/game_adapter.h>
#include <growl/plugins/metal/metal_graphics.h>
#include <growl/plugins/sdl2/sdl_system.h>
#include <iostream>

using Growl::GameAdapter;

GameAdapter::GameAdapter(std::unique_ptr<Game> game,
						 std::unique_ptr<WindowConfig> windowConfig)
	: m_api(std::make_unique<API>()), m_game(std::move(game)),
	  m_window_config(std::move(windowConfig)) {
	std::cout << "Desktop adapter created" << std::endl;

	m_api->systemInternal = std::make_unique<SDL2SystemAPI>();
	m_api->graphicsInternal =
		std::make_unique<MetalGraphicsAPI>(*m_api->system());
	m_game->m_api = m_api.get();

	m_api->systemInternal->init();
	m_api->graphicsInternal->init();
	m_game->init();
}

GameAdapter::~GameAdapter() {
	m_api->graphicsInternal->dispose();
	m_api->systemInternal->dispose();
	std::cout << "Adapter destroyed" << std::endl;
}

void GameAdapter::run() {
	std::cout << "Run!" << std::endl;
	m_api->graphicsInternal->setWindow(*m_window_config);
	while (m_api->systemInternal->isRunning()) {
		m_api->system()->tick();
		m_game->render();
	}
}
