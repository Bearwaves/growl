#include "game.h"
#include "growl/core/api/api.h"
#include "growl/core/error.h"
#include "growl/core/graphics/color.h"
#include "growl/scene/rectangle.h"
#include "growl/ui/label.h"
#include "growl/ui/list.h"
#include "growl/ui/pack.h"
#include "growl/ui/widget.h"

using Growl::Align;
using Growl::AssetsBundle;
using Growl::Color;
using Growl::Error;
using Growl::FontFace;
using Growl::GlyphLayout;
using Growl::Label;
using Growl::List;
using Growl::Node;
using Growl::Rectangle;
using Growl::Result;
using Growl::Value;
using Growl::Widget;
using UIExample::Game;

Error Game::init() {
	getAPI().system().log("Game", "Game starting up!");

	getAPI().system().log("Game", "Loading asset bundle");
	Result<AssetsBundle> bundle_result =
		loadAssetsBundle(getAPI().system(), "assets.growl");
	if (!bundle_result) {
		return std::move(bundle_result.error());
	}

	getAPI().system().log("Game", "Loading font");
	{
		Result<FontFace> font_result =
			bundle_result.get().getDistanceFieldFont("fonts/andada.otf");
		if (!font_result) {
			return std::move(font_result.error());
		}
		font = std::make_unique<FontFace>(std::move(font_result.get()));
		font_atlas = getAPI().graphics().createFontTextureAtlas(*font);
	}
	getAPI().system().log("TestAppGame", "Generating layout");

	batch = getAPI().graphics().createBatch();

	root = std::make_unique<List>("Root", List::Direction::HORIZONTAL);
	getAPI().system().setInputProcessor(root.get());

	auto col = root->addWithLayout<List>("Column", List::Direction::VERTICAL)
				   .fillAcross()
					 .expand()
				   .width(Value::percentWidth(0.2f, root.get()))
				   .getNode();

	label = col->addWithLayout<Label>(
				   "Text", text, *font_atlas, *font,
				   Value::percentHeight(0.03f, col), true)
				.width(Value::percentWidth(1.f, col))
				.expand()
				.getNode();
	root->setDebugRendering(DebugRendering::ON);

	return nullptr;
}

void Game::tick(double delta_time) {}

void Game::render(double delta_time) {
	batch->clear(0, 0, 0);
	batch->begin();

	root->draw(*batch, 1.f);

	batch->end();
}

void Game::resize(const int width, const int height) {
	root->setX(0);
	root->setY(0);
	root->setWidth(width);
	root->setHeight(height);
	root->invalidate();
}

Error Game::dispose() {
	getAPI().system().log("Game", "Game shutting down");
	return nullptr;
}
