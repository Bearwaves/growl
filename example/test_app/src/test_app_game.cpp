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
	counter += getAPI().graphics()->getDeltaTime();
	getAPI().graphics()->clear(0.64, 0.56, 0.51);
	auto batch = getAPI().graphics()->createBatch();
	float progress =
		counter < SPEED ? counter / SPEED : (2 * SPEED - counter) / SPEED;
	batch->begin();
	batch->draw(texture.get(), 0 + (500 * progress), 0, 500, 500);
	batch->draw(texture.get(), 500 - (500 * progress), 500, 500, 500);
	batch->end();
	if (counter > SPEED * 2) {
		counter -= SPEED * 2;
	}
}

void TestAppGame::resize(const int width, const int height) {}

TestAppGame::~TestAppGame() {
	std::cout << "Game destroy" << std::endl;
}
