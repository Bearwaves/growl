#pragma once

#include "growl/core/config.h"
#include "growl/core/error.h"

namespace Growl {

class API;

class Game {

public:
	explicit Game(Config config)
		: config{config} {}
	virtual ~Game() = default;

	virtual Error init() = 0;
	virtual Error dispose() = 0;
	virtual void tick(double delta_time) {}
	virtual void render(double delta_time) = 0;
	virtual void resize(const int width, const int height) = 0;

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	API& getAPI();

	const Config& getConfig() {
		return config;
	}

	void setAPI(API* api) {
		m_api = api;
	}

private:
	API* m_api;
	Config config;
};

} // namespace Growl
