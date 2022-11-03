#include "test_app_game.h"
#include "growl/core/assets/audio.h"
#include "growl/core/assets/bundle.h"
#include "growl/core/assets/font_face.h"
#include "growl/core/assets/image.h"
#include "growl/core/error.h"
#include "growl/core/text/glyph_layout.h"
#include "growl/core/util/timer.h"
#ifdef GROWL_IMGUI
#include "imgui.h"
#endif
#include <memory>
#include <string>

using Growl::Error;
using Growl::TestAppGame;
using Growl::Timer;

Error TestAppGame::init() {
	getAPI().system().log("TestAppGame", "Game starting up!");

	getAPI().system().log("TestAppGame", "Loading asset bundle");
	Result<AssetsBundle> bundle_result =
		loadAssetsBundle(getAPI().system(), "assets.growl");
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
	getAPI().system().setLogLevel(LogLevel::Debug);

	getAPI().system().log("TestAppGame", "Loading texture atlas");
	{
		Timer timer(getAPI().system(), "TestAppGame", "Loading texture atlas");
		Result<Atlas> atlas_result = bundle_result.get().getAtlas("gfx");
		if (atlas_result.hasError()) {
			return std::move(atlas_result.error());
		}
		{
			Timer t(getAPI().system(), "TestAppGame", "Creating texture");
			texture_atlas =
				getAPI().graphics().createTextureAtlas(atlas_result.get());
		}
	}

	getAPI().system().log("TestAppGame", "Loading SFX");
	{
		Timer timer(getAPI().system(), "TestAppGame", "Loading SFX");
		Result<std::unique_ptr<AudioClip>> meow_result =
			getAPI().audio().loadClipFromBundle(
				bundle_result.get(), "sfx/meow.wav");
		if (meow_result.hasError()) {
			return std::move(meow_result.error());
		}
		meow = std::move(meow_result.get());
	}

	getAPI().system().log("TestAppGame", "Loading music");
	{
		Timer timer(getAPI().system(), "TestAppGame", "Loading music");
		Result<std::unique_ptr<AudioStream>> music_result =
			getAPI().audio().createStreamFromBundle(
				bundle_result.get(), "mfx/I pasta way - William Watson.ogg");
		if (music_result.hasError()) {
			return std::move(music_result.error());
		}
		music = std::move(music_result.get());
	}

	return nullptr;
}

void TestAppGame::render() {
#ifdef GROWL_IMGUI
	ImGui::Begin("Growl Test App");
	ImGui::Text(
		"Application average %.2f ms/frame (%.1f FPS)",
		1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderInt("Font size", &font_size, 1, 150);
	ImGui::End();
#endif

	if (!grass_tiled) {
		auto grass_region = texture_atlas->getRegion("grass.png").get();
		// Pre-tile some grass to demo render-to-texture.
		grass_tiled = getAPI().graphics().createTexture(
			grass_region.region.width * 2, grass_region.region.height * 2);

		auto batch = getAPI().graphics().createBatch(*grass_tiled);
		batch->begin();
		for (int x = 0; x < 2; x++) {
			for (int y = 0; y < 2; y++) {
				batch->draw(
					grass_region, x * grass_region.region.width,
					y * grass_region.region.height, grass_region.region.width,
					grass_region.region.height);
			}
		}
		batch->end();
		getAPI().audio().play(*music);
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

	if (font_size != layout->getFontSize()) {
		layout->setFontSize(font_size);
	}
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

Error TestAppGame::dispose() {
	getAPI().system().log("TestAppGame", "Game destroy");
	return nullptr;
}
