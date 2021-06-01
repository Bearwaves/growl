#pragma once

#include <growl/core/game/game.h>

namespace Growl {

class TestAppGame : public Game {
public:
	explicit TestAppGame() = default;
	virtual ~TestAppGame();
	void create() override;
	void render() override;
	void resize(const int width, const int height) override;
};

} // namespace Growl
