#pragma once

#include "growl/core/graphics/window.h"
#include <memory>

namespace Growl {

class Game;

class GameAdapter {
public:
	explicit GameAdapter(
		std::unique_ptr<Game> game, WindowConfig window_config);
	~GameAdapter();
	void run();

private:
	static void doLoopIteration();
	WindowConfig window_config;
};

} // namespace Growl
