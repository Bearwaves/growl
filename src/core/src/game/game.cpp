#include <growl/core/game/game.h>

using Growl::API;
using Growl::Game;

API& Game::getAPI() {
	return *m_api;
}
