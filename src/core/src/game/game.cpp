#include "growl/core/game/game.h"
#include <iostream>

using Growl::API;
using Growl::Game;

API& Game::getAPI() {
	return *m_api;
}
