#include "test_app_game.h"
#include <growl/util/assets/bundle.h>
#include <growl/util/assets/image.h>

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
	Result<Atlas> atlasResult = bundleResult.get().getAtlas("gfx");
	if (atlasResult.hasError()) {
		return std::move(atlasResult.error());
	}
	texture_atlas = getAPI().graphics()->createTextureAtlas(atlasResult.get());

	Result<Image> imageResult = loadImageFromFile("../assets/gfx/grass.png");
	if (imageResult.hasError()) {
		return std::move(imageResult.error());
	}
	grass = getAPI().graphics()->createTexture(imageResult.get());

	return nullptr;
}

void TestAppGame::render() {
	if (!grass_tiled) {
		// Pre-tile some grass to demo render-to-texture.
		grass_tiled = getAPI().graphics()->createTexture(
			grass->getWidth() * 2, grass->getHeight() * 2);

		auto batch = getAPI().graphics()->createBatch(*grass_tiled);
		batch->begin();
		for (int x = 0; x < 2; x++) {
			for (int y = 0; y < 2; y++) {
				batch->draw(
					*grass, x * grass->getWidth(), y * grass->getHeight(),
					grass->getWidth(), grass->getHeight());
			}
		}
		batch->end();
		grass = nullptr;
	}

	counter += getAPI().graphics()->getDeltaTime();
	frames++;
	catX += getAPI().graphics()->getDeltaTime() * SPEED *
			(input->leftPressed() ? -1 : (input->rightPressed() ? 1 : 0));
	catY += getAPI().graphics()->getDeltaTime() * SPEED *
			(input->upPressed() ? -1 : (input->downPressed() ? 1 : 0));
	getAPI().graphics()->clear(0, 0, 0);

	auto batch = getAPI().graphics()->createBatch();
	batch->begin();

	for (int x = 0; x < batch->getTargetWidth(); x += grass_tiled->getWidth()) {
		for (int y = 0; y < batch->getTargetHeight();
			 y += grass_tiled->getHeight()) {
			batch->draw(
				*grass_tiled, x, y, grass_tiled->getWidth(),
				grass_tiled->getHeight());
		}
	}
	batch->draw(
		texture_atlas->getRegion("cat.jpg").get(), catX, catY, 500, 500);
	batch->draw(
		texture_atlas->getRegion("mouse.jpg").get(), input->getMouseX() - 100,
		input->getMouseY() - 100, 200, 200);
	batch->end();
	if (counter > FPS_SAMPLE_SECONDS) {
		getAPI().system()->log("TestAppGame", "FPS: {:05f}", frames / counter);
		counter -= FPS_SAMPLE_SECONDS;
		frames = 0;
	}
}

void TestAppGame::resize(const int width, const int height) {}

TestAppGame::~TestAppGame() {
	getAPI().system()->log("TestAppGame", "Game destroy");
}
