#include "game.h"
#include "growl/core/api/api.h"
#include "growl/core/error.h"
#include "growl/core/graphics/color.h"
#include "growl/scene/rectangle.h"
#include "growl/ui/list.h"
#include "growl/ui/widget.h"

using Growl::Color;
using Growl::Error;
using Growl::List;
using Growl::Rectangle;
using Growl::Widget;
using UIExample::Game;

Error Game::init() {
	getAPI().system().log("Game", "Game starting up!");
	batch = getAPI().graphics().createBatch();

	root = std::make_unique<List>("Root", List::Direction::VERTICAL);
	root->addWithLayout(std::make_unique<Rectangle>("Red", Color{1, 0, 0, 1}))
		.fill()
		.height(50);
	root->addWithLayout(std::make_unique<Rectangle>("Green", Color{0, 1, 0, 1}))
		.fill()
		.height(100);
	root->addWithLayout(std::make_unique<Rectangle>("Blue", Color{0, 0, 1, 1}))
		.fill()
		.height(50);

	auto row = static_cast<Widget*>(
		root->addWithLayout(
				std::make_unique<List>("Row", List::Direction::HORIZONTAL))
			.fill()
			.expand()
			.getNode());
	row->addWithLayout(std::make_unique<Rectangle>("Yellow", Color{1, 1, 0, 1}))
		.height(100)
		.expand();
	row->addWithLayout(
		   std::make_unique<Rectangle>("Magenta", Color{1, 0, 1, 1}))
		.height(200)
		.expand();
	row->addWithLayout(std::make_unique<Rectangle>("Cyan", Color{0, 1, 1, 1}))
		.height(300)
		.expand();

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
