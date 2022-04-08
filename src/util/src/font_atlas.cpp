#include <growl/util/assets/font_atlas.h>

#include "../../../thirdparty/stb_rect_pack/stb_rect_pack.h"
#include "font_internal.h"
#include <cstdio>
#include <freetype/freetype.h>
#include <freetype/ftimage.h>
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

static Error setFontFacePixelSize(Font& font, int size) noexcept;
static Result<FontAtlas>
packFontAtlas(Font& font, std::vector<stbrp_rect>& rects) noexcept;

Result<FontAtlas>
Growl::createFontAtlasFromFont(Font& font, int size) noexcept {
	if (auto err = setFontFacePixelSize(font, size); err) {
		return std::move(err);
	}

	std::vector<stbrp_rect> glyph_rects;
	for (int i = 0; i < font.getFTFontData().face->num_glyphs; i++) {
		if (auto err = FT_Load_Glyph(
				font.getFTFontData().face, i, FT_LOAD_BITMAP_METRICS_ONLY);
			err) {
			return Error(
				std::make_unique<FontError>("Failed to load glyph", err));
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
	if (auto err = setFontFacePixelSize(font, size); err) {
		return std::move(err);
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
			return Error(std::make_unique<FontError>(
				"Failed to load glyph " + code_hex, err));
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

Error setFontFacePixelSize(Font& font, int size) noexcept {
	if (font.getFTFontData().face->num_fixed_sizes) {
		int best = 0;
		int diff = std::abs(
			size - font.getFTFontData().face->available_sizes[0].height);
		for (int i = 1; i < font.getFTFontData().face->num_fixed_sizes; i++) {
			int ndiff = std::abs(
				size - font.getFTFontData().face->available_sizes[i].height);
			if (ndiff < diff) {
				diff = ndiff;
				best = i;
			}
		}
		if (auto err = FT_Select_Size(font.getFTFontData().face, best); err) {
			return Error(std::make_unique<FontError>(
				"Failed to select font face size", err));
		}
		return nullptr;
	}

	if (auto err = FT_Set_Pixel_Sizes(font.getFTFontData().face, 0, size);
		err) {
		return Error(
			std::make_unique<FontError>("Failed to set font face size", err));
	}
	return nullptr;
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
	bool has_color = FT_HAS_COLOR(font.getFTFontData().face);
	int load_params = FT_LOAD_RENDER;
	if (has_color) {
		load_params |= FT_LOAD_COLOR;
	}
	for (auto& rect : glyph_rects) {
		if (auto err =
				FT_Load_Glyph(font.getFTFontData().face, rect.id, load_params);
			err) {
			return Error(std::make_unique<FontError>(
				"Failed to load glyph bitmap", err));
		}
		auto& bitmap = font.getFTFontData().face->glyph->bitmap;
		int bytes_per_pixel = has_color ? 4 : 1;

		for (int row = 0; row < bitmap.rows; row++) {
			for (int col = 0; col < bitmap.width; col++) {
				int x = rect.x + col;
				int y = rect.y + row;
				unsigned char* dst =
					image_data.data() + sizeof(uint32_t) * (y * width + x);
				unsigned char* src =
					bitmap.buffer +
					bytes_per_pixel * (row * bitmap.width + col);
				if (has_color) {
					unsigned char b = *src++, g = *src++, r = *src++,
								  a = *src++;
					*dst++ = r;
					*dst++ = g;
					*dst++ = b;
					*dst++ = a;
				} else {
					*dst++ = 0xFF;
					*dst++ = 0xFF;
					*dst++ = 0xFF;
					*dst++ = *src;
				}
			}
		}
	}

	return FontAtlas(
		font, std::make_unique<Image>(width, height, 4, image_data));
}
