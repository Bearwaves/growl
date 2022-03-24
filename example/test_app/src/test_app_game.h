#pragma once

#include <growl/core/game/game.h>
#include <growl/core/graphics/texture.h>
#include <growl/util/assets/image.h>
#include <memory>
#include <optional>

#include "input.h"
#include <growl/core/graphics/texture_atlas.h>
#include <growl/util/assets/atlas.h>

namespace Growl {

class TestAppGame : public Game {
public:
	explicit TestAppGame() = default;
	virtual ~TestAppGame();
	Error init() override;
	void render() override;
	void resize(const int width, const int height) override;

private:
	std::unique_ptr<Atlas> atlas;
	std::unique_ptr<TextureAtlas> texture_atlas;
	std::unique_ptr<InputHandler> input;
	double counter = 0;
	constexpr const static float SPEED = 500; // px/s
	int frames = 0;
	float catX = 0;
	float catY = 0;
};

} // namespace Growl
