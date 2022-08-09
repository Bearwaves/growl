#include "test_app_game.h"
#include "growl/core/assets/audio.h"
#include "growl/core/assets/bundle.h"
#include "growl/core/assets/font_face.h"
#include "growl/core/assets/image.h"
#include "growl/core/error.h"
#include "growl/core/text/glyph_layout.h"
#include "growl/core/util/timer.h"
#include <memory>
#include <string>

using Growl::Error;
using Growl::TestAppGame;
using Growl::Timer;

Error TestAppGame::init() {
	getAPI().system().log("TestAppGame", "Game starting up!");

	getAPI().system().log("TestAppGame", "Loading asset bundle");
	Result<AssetsBundle> bundle_result = loadAssetsBundle("./assets.growl");
	if (bundle_result.hasError()) {
		return std::move(bundle_result.error());
	}

	getAPI().system().log("TestAppGame", "Loading font");
	{
		Timer timer(getAPI().system(), "TestAppGame", "Generating font atlas");
		Result<FontFace> font_result =
			bundle_result.get().getDistanceFieldFont("fonts/andada.otf");
		if (font_result.hasError()) {
			return std::move(font_result.error());
		}
		font = std::make_unique<FontFace>(std::move(font_result.get()));
		font_atlas = getAPI().graphics().createFontTextureAtlas(*font);
	}
	getAPI().system().log("TestAppGame", "Generating layout");
	layout = std::make_unique<GlyphLayout>(*font, "Hello Growl!", 0, 50);

	input = std::make_unique<InputHandler>(getAPI().system());
	getAPI().system().setInputProcessor(input.get());
	getAPI().system().setLogLevel(LogLevel::DEBUG);

	Result<Atlas> atlas_result = bundle_result.get().getAtlas("gfx");
	if (atlas_result.hasError()) {
		return std::move(atlas_result.error());
	}
	texture_atlas = getAPI().graphics().createTextureAtlas(atlas_result.get());

	Result<Image> image_result = loadImageFromFile("../assets/gfx/grass.png");
	if (image_result.hasError()) {
		return std::move(image_result.error());
	}
	grass = getAPI().graphics().createTexture(image_result.get());

	Result<std::unique_ptr<AudioClip>> meow_result =
		getAPI().audio().loadClipFromBundle(
			bundle_result.get(), "sfx/meow.wav");
	if (meow_result.hasError()) {
		return std::move(meow_result.error());
	}
	meow = std::move(meow_result.get());

	return nullptr;
}

void TestAppGame::render() {
	if (!grass_tiled) {
		// Pre-tile some grass to demo render-to-texture.
		grass_tiled = getAPI().graphics().createTexture(
			grass->getWidth() * 2, grass->getHeight() * 2);

		auto batch = getAPI().graphics().createBatch(*grass_tiled);
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

	counter += getAPI().graphics().getDeltaTime();
	frames++;
	catX += getAPI().graphics().getDeltaTime() * SPEED *
			(input->leftPressed() ? -1 : (input->rightPressed() ? 1 : 0));
	catY += getAPI().graphics().getDeltaTime() * SPEED *
			(input->upPressed() ? -1 : (input->downPressed() ? 1 : 0));
	// If cat catches mouse, meow
	if (catX < input->getMouseX() + 100 &&
		catX + 500 > input->getMouseX() - 100 &&
		catY < input->getMouseY() + 100 &&
		catY + 500 > input->getMouseY() - 100) {
		if (!caught) {
			caught = true;
			getAPI().audio().play(*meow);
		}
	} else {
		caught = false;
	}

	getAPI().graphics().clear(0, 0, 0);

	auto batch = getAPI().graphics().createBatch();
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
	batch->draw(
		*layout, *font_atlas,
		batch->getTargetWidth() - (layout->getWidth() * 1.05), 50);
	batch->end();
	if (counter > FPS_SAMPLE_SECONDS) {
		layout->setText("FPS: " + std::to_string(frames / counter));
		counter -= FPS_SAMPLE_SECONDS;
		frames = 0;
	}
}

void TestAppGame::resize(const int width, const int height) {}

TestAppGame::~TestAppGame() {
	getAPI().system().log("TestAppGame", "Game destroy");
}
