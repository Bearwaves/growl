#include "test_app_game.h"
#include <growl/platforms/desktop/game_adapter.h>
#include <memory>

using namespace Growl;

int main() {
	auto adapter = GameAdapter{std::make_unique<TestAppGame>()};
	adapter.run();
}
