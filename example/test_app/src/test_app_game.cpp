#include "test_app_game.h"
#include <growl/util/assets/bundle.h>

using Growl::Error;
using Growl::TestAppGame;

Error TestAppGame::init() {
	input = std::make_unique<InputHandler>(getAPI().system());
	getAPI().system()->setInputProcessor(input.get());
	getAPI().system()->setLogLevel(LogLevel::DEBUG);
	getAPI().system()->log("TestAppGame", "Game starting up!");
	Result<AssetsBundle> bundleResult = loadAssetsBundle("./assets.growl");
	if (bundleResult.hasError()) {
		return std::move(bundleResult.error());
	}
	Result<Image> imageResult = bundleResult.get().getImage("gfx/cat.jpg");
	if (imageResult.hasError()) {
		return std::move(imageResult.error());
	}
	image = std::move(imageResult.get());
	getAPI().system()->log(
		LogLevel::DEBUG, "TestAppGame", "Got image! W {}, H {}, Ch {}",
		image->getWidth(), image->getHeight(), image->getChannels());
	texture = getAPI().graphics()->createTexture(*image);

	return nullptr;
}

void TestAppGame::render() {
	counter += getAPI().graphics()->getDeltaTime();
	frames++;
	catX += getAPI().graphics()->getDeltaTime() * SPEED *
			(input->leftPressed() ? -1 : (input->rightPressed() ? 1 : 0));
	catY += getAPI().graphics()->getDeltaTime() * SPEED *
			(input->upPressed() ? -1 : (input->downPressed() ? 1 : 0));
	getAPI().graphics()->clear(0.64, 0.56, 0.51);
	auto batch = getAPI().graphics()->createBatch();
	batch->begin();
	batch->draw(
		texture.get(), input->getMouseX() - 100, input->getMouseY() - 100, 200,
		200);
	batch->draw(texture.get(), catX, catY, 500, 500);
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
