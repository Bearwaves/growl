#pragma once

#include <growl/core/game/game.h>
#include <growl/core/graphics/texture.h>
#include <growl/util/assets/image.h>
#include <memory>
#include <optional>

#include "input.h"

namespace Growl {

class TestAppGame : public Game {
public:
	explicit TestAppGame() = default;
	virtual ~TestAppGame();
	Error init() override;
	void render() override;
	void resize(const int width, const int height) override;

private:
	std::unique_ptr<Texture> catTexture;
	std::unique_ptr<Texture> mouseTexture;
	std::unique_ptr<InputHandler> input;
	std::optional<Image> catImage;
	std::optional<Image> mouseImage;
	double counter = 0;
	constexpr const static float SPEED = 500; // px/s
	int frames = 0;
	float catX;
	float catY;
};

} // namespace Growl
