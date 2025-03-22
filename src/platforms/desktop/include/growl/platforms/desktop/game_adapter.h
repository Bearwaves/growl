#pragma once

#include <memory>

namespace Growl {

class Game;
class API;

class GameAdapter {
public:
	explicit GameAdapter(std::unique_ptr<Game> game);
	~GameAdapter();
	void run();

private:
	std::unique_ptr<API> m_api;
	std::unique_ptr<Game> m_game;
};

} // namespace Growl
