#pragma once

#include <memory>

namespace Growl {

class Game;

class GameAdapter {
public:
	explicit GameAdapter(std::unique_ptr<Game> game);
	~GameAdapter();
	void run();

private:
	static void doLoopIteration();
};

} // namespace Growl
