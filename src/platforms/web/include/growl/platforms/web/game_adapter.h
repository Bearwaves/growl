#pragma once

#include "growl/core/config.h"
#include <memory>

namespace Growl {

class Game;

class GameAdapter {
public:
	explicit GameAdapter(std::unique_ptr<Game> game, Config config);
	~GameAdapter();
	void run();

private:
	static void doLoopIteration();
	Config config;
};

} // namespace Growl
