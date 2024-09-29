#include "game.h"
#include "growl/core/api/api.h"
#include "growl/core/error.h"
#include "growl/core/graphics/color.h"
#include "growl/scene/rectangle.h"
#include "growl/ui/list.h"

using Growl::Color;
using Growl::Error;
using Growl::List;
using Growl::Rectangle;
using UIExample::Game;

Error Game::init() {
	getAPI().system().log("Game", "Game starting up!");
	batch = getAPI().graphics().createBatch();

	root = std::make_unique<List>("Root", List::Direction::VERTICAL);
	root->addChild(std::make_unique<Rectangle>("Red", Color{1, 0, 0, 1}));
	root->addChild(std::make_unique<Rectangle>("Green", Color{0, 1, 0, 1}));
	root->addChild(std::make_unique<Rectangle>("Blue", Color{0, 0, 1, 1}));

	auto* row = root->addChild(
		std::make_unique<List>("Row", List::Direction::HORIZONTAL));
	row->addChild(std::make_unique<Rectangle>("Yellow", Color{1, 1, 0, 1}));
	row->addChild(std::make_unique<Rectangle>("Magenta", Color{1, 0, 1, 1}));
	row->addChild(std::make_unique<Rectangle>("Cyan", Color{0, 1, 1, 1}));

	auto* col = row->addChild(
		std::make_unique<List>("Column", List::Direction::HORIZONTAL));
	col->addChild(
		std::make_unique<Rectangle>("Light grey", Color{.8f, .8f, .8f, 1}));
	col->addChild(
		std::make_unique<Rectangle>("Dark grey", Color{.2f, .2f, .2f, 1}));

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
