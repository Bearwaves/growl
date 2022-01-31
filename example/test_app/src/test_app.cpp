#include "test_app_game.h"
#include <growl/core/graphics/window.h>
#include <growl/platforms/desktop/game_adapter.h>
#include <memory>

int main() {
	auto adapter = Growl::GameAdapter{
		std::make_unique<Growl::TestAppGame>(),
		Growl::WindowConfig{"Test App", 500, 500, true}};
	adapter.run();
}
