#pragma once

#include "growl/core/game/game.h"
#include "growl/core/graphics/batch.h"
#include "growl/ui/label.h"
#include "growl/ui/text_input.h"
#include "growl/ui/widget.h"

namespace UIExample {

class Game : public Growl::Game {
public:
	Game(Growl::Config config)
		: Growl::Game{config} {}
	Growl::Error init() override;
	Growl::Error dispose() override;
	void tick(double delta_time) override;
	void render(double delta_time) override;
	void resize(const int width, const int height) override;

private:
	std::unique_ptr<Growl::Batch> batch;
	std::unique_ptr<Growl::Widget> root;
	std::unique_ptr<Growl::FontFace> font;
	std::unique_ptr<Growl::FontTextureAtlas> font_atlas;
	Growl::Label* label;
	std::string text = "Hello this is some example text in Growl.\n\nIt's got "
					   "newlines and everything.";
	Growl::TextInput* input;
};

} // namespace UIExample
