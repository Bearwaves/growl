#include "test_app_game.h"
#include "cat.h"
#include "growl/core/api/api.h"
#include "growl/core/assets/audio.h"
#include "growl/core/assets/bundle.h"
#include "growl/core/assets/font_face.h"
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

	cats = std::make_unique<Node>();
	cats->setWidth(500);
	cats->setHeight(500);
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			auto inner_cats = std::make_unique<Node>();
			inner_cats->setWidth(250);
			inner_cats->setHeight(250);
			inner_cats->setX(250 * i);
			inner_cats->setY(250 * j);
			for (int ii = 0; ii < 10; ii++) {
				for (int jj = 0; jj < 10; jj++) {
					auto cat = std::make_unique<Cat>(texture_atlas.get());
					cat->setWidth(25);
					cat->setHeight(25);
					cat->setX(25 * ii);
					cat->setY(25 * jj);
					inner_cats->addChild(std::move(cat));
				}
			}
			cats->addChild(std::move(inner_cats));
		}
	}

	input = std::make_unique<InputHandler>(getAPI().system(), cats.get());
	getAPI().system().setInputProcessor(input.get());
	getAPI().system().setLogLevel(LogLevel::Debug);

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

	float rotation = cats->getRotation();
	rotation +=
		getAPI().graphics().getDeltaTime() * SPEED * 0.01f *
		(input->anticlockwisePressed() ? -1
									   : (input->clockwisePressed() ? 1 : 0));
	cats->setX(catX);
	cats->setY(catY);
	cats->setRotation(rotation);
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

	auto batch = getAPI().graphics().createBatch();
	batch->clear(0, 0, 0);
	batch->begin();

	for (int x = 0; x < batch->getTargetWidth(); x += grass_tiled->getWidth()) {
		for (int y = 0; y < batch->getTargetHeight();
			 y += grass_tiled->getHeight()) {
			batch->draw(
				*grass_tiled, x, y, grass_tiled->getWidth(),
				grass_tiled->getHeight());
		}
	}
	cats->draw(*batch, 1);
	/*batch->draw(
		texture_atlas->getRegion("mouse.jpg").get(), input->getMouseX() - 100,
		input->getMouseY() - 100, 200, 200);*/

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

void TestAppGame::resize(const int width, const int height) {
	getAPI().system().log(
		"TestAppGame", "Window resized: ({}, {})", width, height);
}

Error TestAppGame::dispose() {
	getAPI().system().log("TestAppGame", "Game destroy");
	return nullptr;
}

void Growl::Cat::draw(Batch& batch, float parent_alpha) {
	batch.setColor(1, !is_hit, !is_hit, parent_alpha);
	batch.draw(
		atlas->getRegion("cat.jpg").get(), getX(), getY(), getWidth(),
		getHeight());
	batch.setColor(1, 1, 1, parent_alpha);
}

void Growl::Cat::onMouseEvent(InputMouseEvent& event) {
	is_hit = hit(event.mouseX, event.mouseY);
}
