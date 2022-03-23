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
	Result<Image> catImageResult = bundleResult.get().getImage("gfx/cat.jpg");
	if (catImageResult.hasError()) {
		return std::move(catImageResult.error());
	}
	catImage = std::move(catImageResult.get());
	getAPI().system()->log(
		LogLevel::DEBUG, "TestAppGame", "Got cat image! W {}, H {}, Ch {}",
		catImage->getWidth(), catImage->getHeight(), catImage->getChannels());
	catTexture = getAPI().graphics()->createTexture(*catImage);

	Result<Image> mouseImageResult =
		bundleResult.get().getImage("gfx/mouse.jpg");
	if (mouseImageResult.hasError()) {
		return std::move(mouseImageResult.error());
	}
	mouseImage = std::move(mouseImageResult.get());
	getAPI().system()->log(
		LogLevel::DEBUG, "TestAppGame", "Got mouse image! W {}, H {}, Ch {}",
		mouseImage->getWidth(), mouseImage->getHeight(),
		mouseImage->getChannels());
	mouseTexture = getAPI().graphics()->createTexture(*mouseImage);

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
	batch->draw(*catTexture, catX, catY, 500, 500);
	batch->draw(
		*mouseTexture, input->getMouseX() - 100, input->getMouseY() - 100, 200,
		200);
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
