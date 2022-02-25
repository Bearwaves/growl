#include <chrono>
#include <growl/core/graphics/window.h>
#include <growl/platforms/desktop/game_adapter.h>

using Growl::API;
using Growl::GameAdapter;

void initSDL2Plugin(API& api);
void initMetalPlugin(API& api);

GameAdapter::GameAdapter(std::unique_ptr<Game> game, WindowConfig windowConfig)
	: m_api(std::make_unique<API>())
	, m_game(std::move(game))
	, m_window_config(std::move(windowConfig)) {

	initSDL2Plugin(*m_api);
	initMetalPlugin(*m_api);
	m_game->m_api = m_api.get();

	m_api->systemInternal->init();
	m_api->graphicsInternal->init();
	m_api->system()->log("GameAdapter", "Desktop adapter created");
}

GameAdapter::~GameAdapter() {
	m_api->system()->log("GameAdapter", "Desktop adapter destroying");
	m_api->graphicsInternal->dispose();
	m_api->systemInternal->dispose();
}

void GameAdapter::run() {
	m_api->graphicsInternal->setWindow(m_window_config);
	m_game->init();
	m_api->system()->log("GameAdapter", "Run!");
	while (m_api->systemInternal->isRunning()) {
		m_api->system()->tick();
		m_api->graphicsInternal->begin();
		m_game->render();
		m_api->graphicsInternal->end();
	}
}
