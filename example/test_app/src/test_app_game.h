#pragma once

#include <growl/core/game/game.h>
#include <growl/core/graphics/texture.h>
#include <growl/util/image/image.h>
#include <memory>

namespace Growl {

class TestAppGame : public Game {
public:
	explicit TestAppGame() = default;
	virtual ~TestAppGame();
	void init() override;
	void render() override;
	void resize(const int width, const int height) override;

private:
	std::unique_ptr<Texture> texture;
	std::unique_ptr<Image> image;
	double counter = 0;
	constexpr const static float SPEED = 2;
	int frames = 0;
};

} // namespace Growl
