#pragma once

#include "growl/core/assets/audio.h"
#include "growl/core/assets/font_face.h"
#include "growl/core/error.h"
#include "growl/core/game/game.h"
#include "growl/core/graphics/font_texture_atlas.h"
#include "growl/core/graphics/texture.h"
#include "growl/core/graphics/texture_atlas.h"
#include "growl/core/text/glyph_layout.h"
#include <memory>

#include "growl/scene/node.h"
#include "input.h"

namespace Growl {

constexpr int FPS_SAMPLE_SECONDS = 1;

class TestAppGame : public Game {
public:
	explicit TestAppGame() = default;
	Error init() override;
	Error dispose() override;
	void render() override;
	void resize(const int width, const int height) override;

private:
	std::unique_ptr<TextureAtlas> texture_atlas;
	std::unique_ptr<Texture> grass_tiled;
	std::unique_ptr<InputHandler> input;

	std::unique_ptr<FontFace> font;
	std::unique_ptr<FontTextureAtlas> font_atlas;
	std::unique_ptr<GlyphLayout> layout;

	std::unique_ptr<AudioClip> meow;
	std::unique_ptr<AudioStream> music;
	std::unique_ptr<Node> cats;

	double counter = 0;
	constexpr const static float SPEED = 500; // px/s
	int frames = 0;
	float catX = 0;
	float catY = 0;
	bool caught = false;
	int font_size = 50;
};

} // namespace Growl
