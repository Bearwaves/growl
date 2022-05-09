#include "growl/util/assets/font_atlas.h"

#include "../../../thirdparty/stb_rect_pack/stb_rect_pack.h"
#include "font_internal.h"
#include "freetype/freetype.h"
#include "freetype/ftimage.h"
// Must go after FreeType.
#include "../../../thirdparty/msdfgen/msdfgen-ext.h"
#include "../../../thirdparty/msdfgen/msdfgen.h"
#include "growl/util/assets/error.h"
#include "growl/util/assets/image.h"
#include "growl/util/error.h"
#include <cstdio>
#include <memory>
#include <set>
#include <unordered_map>
#include <utf8.h>
#include <vector>

using Growl::AssetsError;
using Growl::Error;
using Growl::Font;
using Growl::FontAtlas;
using Growl::FontAtlasType;
using Growl::GlyphPosition;
using Growl::Image;
using Growl::Result;

constexpr int MAX_SIZE = 8192;
constexpr int SPACING = 1;

static int nextPowerOfTwo(int n) {
	int i = 2;
	while (i < n) {
		i *= 2;
	}
	return i;
}

static Error setFontFacePixelSize(Font& font, int size) noexcept;
static bool packRectsIncreasing(
	std::vector<stbrp_rect>& rects, int start_size, int* out_width,
	int* out_height);
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
			i, static_cast<stbrp_coord>(metrics.width >> 6) + SPACING * 2,
			static_cast<stbrp_coord>(metrics.height >> 6) + SPACING * 2});
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
		std::snprintf(hex, sizeof(hex), "0x%04X", uint16_t(code));
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
			glyph_index,
			static_cast<stbrp_coord>(metrics.width >> 6) + SPACING * 2,
			static_cast<stbrp_coord>(metrics.height >> 6) + SPACING * 2});
	}

	return packFontAtlas(font, glyph_rects);
}

// Distance field font implementation is incomplete.
Result<FontAtlas>
Growl::createDistanceFieldFontAtlasFromFont(Font& font) noexcept {
	if (FT_HAS_COLOR(font.getFTFontData().face)) {
		return Error(std::make_unique<AssetsError>(
			"Failed to create distance field font atlas: cannot create from "
			"coloured font."));
	}
	if (auto err = setFontFacePixelSize(font, 32); err) {
		return err;
	}
	msdfgen::FontHandle* font_handle =
		msdfgen::adoptFreetypeFont(font.getFTFontData().face);
	msdfgen::FontMetrics font_metrics;
	msdfgen::getFontMetrics(font_metrics, font_handle);

	float scale = 32 / font_metrics.emSize;
	int border = 2;
	int pack_border = 1;
	float range = 2;

	std::vector<stbrp_rect> glyph_rects;
	for (int i = 0; i < font.getFTFontData().face->num_glyphs; i++) {
		msdfgen::Shape shape;
		// int glyph = FT_Get_Char_Index(font.getFTFontData().face, 0x41);
		msdfgen::loadGlyph(shape, font_handle, msdfgen::GlyphIndex(i));
		if (shape.validate() && shape.contours.size() > 0) {
			shape.inverseYAxis = true;
			shape.normalize();
			shape.orientContours();

			auto bounds = shape.getBounds(border);
			int glyph_width = round((bounds.r - bounds.l) * scale);
			int glyph_height = round((bounds.t - bounds.b) * scale);
			glyph_rects.push_back(stbrp_rect{
				i, glyph_width + (pack_border * 2),
				glyph_height + (pack_border * 2)});
		}
	}

	int width, height;
	if (!packRectsIncreasing(
			glyph_rects,
			nextPowerOfTwo(std::max(glyph_rects[0].w, glyph_rects[0].h)),
			&width, &height)) {
		return Error(std::make_unique<AssetsError>(
			"Failed to pack font in texture; too large"));
	}

	std::unordered_map<int, GlyphPosition> glyphs;
	std::vector<unsigned char> image_data(width * height * 4, 0);
	for (const auto& rect : glyph_rects) {
		msdfgen::Shape shape;
		msdfgen::loadGlyph(shape, font_handle, msdfgen::GlyphIndex(rect.id));
		if (shape.validate() && shape.contours.size() > 0) {
			shape.inverseYAxis = true;
			shape.normalize();
			shape.orientContours();

			auto bounds = shape.getBounds(border);
			msdfgen::edgeColoringSimple(shape, 3);
			msdfgen::Bitmap<float, 4> bitmap(
				rect.w - (pack_border * 2), rect.h - (pack_border * 2));
			msdfgen::generateMTSDF(
				bitmap, shape, range, scale,
				msdfgen::Vector2(-bounds.l, -bounds.b));

			for (int row = 0; row < bitmap.height(); row++) {
				for (int col = 0; col < bitmap.width(); col++) {
					int x = rect.x + pack_border + col;
					int y = rect.y + pack_border + row;
					unsigned char* dst =
						image_data.data() + 4 * (y * width + x);
					*dst++ = msdfgen::pixelFloatToByte(bitmap(col, row)[0]);
					*dst++ = msdfgen::pixelFloatToByte(bitmap(col, row)[1]);
					*dst++ = msdfgen::pixelFloatToByte(bitmap(col, row)[2]);
					*dst++ = msdfgen::pixelFloatToByte(bitmap(col, row)[3]);
				}
			}

			int border_calc = floor(border * scale);
			glyphs[rect.id] = GlyphPosition{
				rect.x + pack_border + border_calc,
				rect.y + pack_border + border_calc,
				rect.w - ((pack_border + border_calc) * 2),
				rect.h - ((pack_border + border_calc) * 2)};
		}
	}

	msdfgen::destroyFont(font_handle);
	return FontAtlas(
		FontAtlasType::MSDF, font,
		std::make_unique<Image>(width, height, 4, image_data),
		std::move(glyphs));
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

bool packRectsIncreasing(
	std::vector<stbrp_rect>& rects, int start_size, int* out_width,
	int* out_height) {
	int width = start_size;
	int height = width;
	while (width <= MAX_SIZE && height <= MAX_SIZE) {
		stbrp_context ctx;
		std::vector<stbrp_node> nodes(width * 2);
		stbrp_init_target(
			&ctx, width, height, nodes.data(), static_cast<int>(nodes.size()));
		if (stbrp_pack_rects(
				&ctx, rects.data(), static_cast<int>(rects.size()))) {
			*out_width = width;
			*out_height = height;
			return true;
		}
		if (width <= height) {
			width *= 2;
		} else {
			height *= 2;
		}
	}
	return false;
}

Result<FontAtlas>
packFontAtlas(Font& font, std::vector<stbrp_rect>& glyph_rects) noexcept {
	int width, height;
	if (!packRectsIncreasing(
			glyph_rects,
			nextPowerOfTwo(std::max(glyph_rects[0].w, glyph_rects[0].h)),
			&width, &height)) {
		return Error(std::make_unique<AssetsError>(
			"Failed to pack font in texture; too large"));
	}

	std::unordered_map<int, GlyphPosition> glyphs;
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

		for (unsigned int row = 0; row < bitmap.rows; row++) {
			for (unsigned int col = 0; col < bitmap.width; col++) {
				int x = rect.x + col + SPACING;
				int y = rect.y + row + SPACING;
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

		glyphs[rect.id] = GlyphPosition{
			rect.x + SPACING, rect.y + SPACING, rect.w - SPACING * 2,
			rect.h - SPACING * 2};
	}

	return FontAtlas(
		FontAtlasType::RGBA, font,
		std::make_unique<Image>(width, height, 4, image_data),
		std::move(glyphs));
}
