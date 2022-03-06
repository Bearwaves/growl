#include "test_app_game.h"

using Growl::TestAppGame;

void TestAppGame::init() {
	getAPI().system()->setLogLevel(LogLevel::DEBUG);
	getAPI().system()->log("TestAppGame", "Game starting up!");
	image = loadImageFromFile("../assets/gfx/cat.jpg");
	getAPI().system()->log(
		LogLevel::DEBUG, "TestAppGame", "Got image! W {}, H {}, Ch {}",
		image->getWidth(), image->getHeight(), image->getChannels());
	texture = getAPI().graphics()->createTexture(image.get());
}

void TestAppGame::render() {
	counter += getAPI().graphics()->getDeltaTime();
	frames++;
	getAPI().graphics()->clear(0.64, 0.56, 0.51);
	auto batch = getAPI().graphics()->createBatch();
	float progress =
		counter < SPEED ? counter / SPEED : (2 * SPEED - counter) / SPEED;
	batch->begin();
	batch->draw(texture.get(), 0 + (500 * progress), 0, 500, 500);
	batch->draw(texture.get(), 500 - (500 * progress), 500, 500, 500);
	batch->end();
	if (counter > SPEED * 2) {
		getAPI().system()->log("TestAppGame", "FPS: {:05f}", frames / counter);
		counter -= SPEED * 2;
		frames = 0;
	}
}

void TestAppGame::resize(const int width, const int height) {}

TestAppGame::~TestAppGame() {
	getAPI().system()->log("TestAppGame", "Game destroy");
}
