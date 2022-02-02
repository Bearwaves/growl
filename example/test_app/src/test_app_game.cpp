#include "test_app_game.h"

#include <iostream>

using Growl::TestAppGame;

void TestAppGame::init() {
	std::cout << "Game starting up!" << std::endl;
	image = loadImageFromFile("/Users/joel/Code/Bearwaves/feud/java/android/"
							  "assets/gfx/decorations.png");
	std::cout << "Got image! W " << image->getWidth() << ", H "
			  << image->getHeight() << ", Ch " << image->getChannels()
			  << std::endl;
	texture = getAPI().graphics()->createTexture(image.get());
}

void TestAppGame::render() {
	getAPI().graphics()->clear(1, 0, 0);
	auto batch = getAPI().graphics()->createBatch();
	batch->begin();
	batch->draw(texture.get(), 0, 0, 1, 1);
	batch->end();
}

void TestAppGame::resize(const int width, const int height) {}

TestAppGame::~TestAppGame() {
	std::cout << "Game destroy" << std::endl;
}
