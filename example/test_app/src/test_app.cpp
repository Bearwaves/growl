#include "test_app_game.h"
#include <growl/platforms/desktop/game_adapter.h>
#include <memory>

int main() {
	auto adapter = Growl::GameAdapter{std::make_unique<Growl::TestAppGame>()};
	adapter.run();
}
