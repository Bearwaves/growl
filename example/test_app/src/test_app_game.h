#pragma once

#include <growl/core/game/game.h>
#include <growl/core/graphics/texture.h>
#include <growl/util/assets/image.h>
#include <memory>
#include <optional>

namespace Growl {

class TestAppGame : public Game {
public:
	explicit TestAppGame() = default;
	virtual ~TestAppGame();
	Error init() override;
	void render() override;
	void resize(const int width, const int height) override;

private:
	std::unique_ptr<Texture> texture;
	std::optional<Image> image;
	double counter = 0;
	constexpr const static float SPEED = 2;
	int frames = 0;
};

} // namespace Growl
