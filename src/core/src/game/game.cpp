#include "growl/core/game/game.h"
#include <iostream>

using namespace Growl;

API& Game::getAPI() {
	return *m_api;
}
