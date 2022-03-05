#include "test_app_game.h"
#include <growl/core/graphics/window.h>
#include <growl/platforms/desktop/game_adapter.h>
#include <memory>

int main() {
	auto adapter = Growl::GameAdapter{
		std::make_unique<Growl::TestAppGame>(),
		Growl::WindowConfig{"Growl Test App", 1000, 1000, true}};
	adapter.run();
}
