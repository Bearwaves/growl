#pragma once

#include "growl/core/api/api.h"
#include "growl/core/error.h"
#include <memory>

namespace Growl {

class GameAdapter;

class Game {

	friend class GameAdapter;

public:
	explicit Game() = default;
	virtual ~Game() = default;

	virtual Error init() = 0;
	virtual Error dispose() = 0;
	virtual void render() = 0;
	virtual void resize(const int width, const int height) = 0;

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

protected:
	API& getAPI();

private:
	API* m_api;
};

} // namespace Growl
