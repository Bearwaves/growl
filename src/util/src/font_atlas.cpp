#include <growl/util/assets/font_atlas.h>

#include "../../../thirdparty/stb_rect_pack/stb_rect_pack.h"
#include "font_internal.h"
#include <cstdio>
#include <freetype/freetype.h>
#include <growl/util/assets/error.h>
#include <growl/util/assets/image.h>
#include <growl/util/error.h>
#include <memory>
#include <set>
#include <utf8.h>

using Growl::AssetsError;
using Growl::Error;
using Growl::Font;
using Growl::FontAtlas;
using Growl::Image;
using Growl::Result;

constexpr int MAX_SIZE = 8192;

static int nextPowerOfTwo(int n) {
	int i = 2;
	while (i < n) {
		i *= 2;
	}
	return i;
}

static Result<FontAtlas>
packFontAtlas(Font& font, std::vector<stbrp_rect>& rects) noexcept;

Result<FontAtlas>
Growl::createFontAtlasFromFont(Font& font, int size) noexcept {
	if (auto err = FT_Set_Pixel_Sizes(font.getFTFontData().face, 0, size);
		err) {
		return Error(
			std::make_unique<AssetsError>("Failed to set font face size"));
	}

	std::vector<stbrp_rect> glyph_rects;
	for (int i = 0; i < font.getFTFontData().face->num_glyphs; i++) {
		if (auto err = FT_Load_Glyph(
				font.getFTFontData().face, i, FT_LOAD_BITMAP_METRICS_ONLY);
			err) {
			return Error(std::make_unique<AssetsError>("Failed to load glyph"));
		}
		auto& metrics = font.getFTFontData().face->glyph->metrics;
		// Glyph sizes are represented in 26.6 fractional format, so we shift
		// them to get the pixel sizes.
		glyph_rects.push_back(stbrp_rect{
			i, static_cast<stbrp_coord>(metrics.width >> 6),
			static_cast<stbrp_coord>(metrics.height >> 6)});
	}

	return packFontAtlas(font, glyph_rects);
}

Result<FontAtlas> Growl::createFontAtlasFromFont(
	Font& font, int size, std::string text) noexcept {
	if (text.empty()) {
		return Error(std::make_unique<AssetsError>(
			"Font atlas charset must not be empty"));
	}
	if (auto end_it = utf8::find_invalid(text.begin(), text.end());
		end_it != text.end()) {
		return Error(std::make_unique<AssetsError>(
			"Font atlas charset not a valid UTF-8 string"));
	}
	if (auto err = FT_Set_Pixel_Sizes(font.getFTFontData().face, 0, size);
		err) {
		return Error(
			std::make_unique<AssetsError>("Failed to set font face size"));
	}

	std::vector<stbrp_rect> glyph_rects;
	std::set<int> glyphs_seen;
	auto iter = text.begin();
	while (iter != text.end()) {
		int code = utf8::next(iter, text.end());
		char hex[7];
		sprintf(hex, "0x%04X", uint16_t(code));
		std::string code_hex(hex);
		int glyph_index = FT_Get_Char_Index(font.getFTFontData().face, code);
		if (!glyph_index) {
			return Error(std::make_unique<AssetsError>(
				"Code point not present in font: " + code_hex));
		}
		if (auto [_, inserted] = glyphs_seen.insert(glyph_index); !inserted) {
			// Already seen glyph
			continue;
		}

		if (auto err = FT_Load_Glyph(
				font.getFTFontData().face, glyph_index,
				FT_LOAD_BITMAP_METRICS_ONLY);
			err) {
			return Error(std::make_unique<AssetsError>(
				"Failed to load glyph " + code_hex));
		}
		auto& metrics = font.getFTFontData().face->glyph->metrics;
		// Glyph sizes are represented in 26.6 fractional format, so we shift
		// them to get the pixel sizes.
		glyph_rects.push_back(stbrp_rect{
			glyph_index, static_cast<stbrp_coord>(metrics.width >> 6),
			static_cast<stbrp_coord>(metrics.height >> 6)});
	}

	return packFontAtlas(font, glyph_rects);
}

Result<FontAtlas>
packFontAtlas(Font& font, std::vector<stbrp_rect>& glyph_rects) noexcept {
	int width = nextPowerOfTwo(std::max(glyph_rects[0].w, glyph_rects[0].h));
	int height = width;
	bool packed = false;
	while (width <= MAX_SIZE && height <= MAX_SIZE) {
		stbrp_context ctx;
		std::vector<stbrp_node> nodes(width * 2);
		stbrp_init_target(&ctx, width, height, nodes.data(), nodes.size());
		if (stbrp_pack_rects(&ctx, glyph_rects.data(), glyph_rects.size())) {
			packed = true;
			break;
		}
		if (width <= height) {
			width *= 2;
		} else {
			height *= 2;
		}
	}
	if (!packed) {
		return Error(std::make_unique<AssetsError>(
			"Failed to pack font in texture; too large"));
	}

	std::vector<unsigned char> image_data(width * height * sizeof(uint32_t), 0);
	uint32_t* image_32 = reinterpret_cast<uint32_t*>(image_data.data());
	for (auto& rect : glyph_rects) {
		if (auto err = FT_Load_Glyph(
				font.getFTFontData().face, rect.id, FT_LOAD_RENDER);
			err) {
			return Error(
				std::make_unique<AssetsError>("Failed to load glyph bitmap"));
		}
		auto& bitmap = font.getFTFontData().face->glyph->bitmap;

		for (int row = 0; row < bitmap.rows; row++) {
			for (int col = 0; col < bitmap.width; col++) {
				int x = rect.x + col;
				int y = rect.y + row;
				if (unsigned char c = bitmap.buffer[row * bitmap.pitch + col];
					c) {
					image_32[y * width + x] =
						0x00FFFFFF | (c << 24); // little endian
				}
			}
		}
	}

	return FontAtlas(
		font, std::make_unique<Image>(width, height, 4, image_data));
}
