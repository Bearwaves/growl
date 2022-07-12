#include "../../../thirdparty/stb_rect_pack/stb_rect_pack.h"
#include "font_internal.h"
#include "growl/util/assets/atlas.h"
#include "growl/util/assets/error.h"
#include "growl/util/assets/font_face.h"
#include "growl/util/assets/image.h"
#include "growl/util/error.h"
#include "hb-ft.h"
#include <memory>
#include <set>
#include <utf8.h>

using Growl::AssetsError;
using Growl::AtlasRegion;
using Growl::Error;
using Growl::FontFace;
using Growl::FontFaceType;
using Growl::FTFontData;
using Growl::Image;
using Growl::Result;

constexpr int SPACING = 1;

Result<FontFace>
createBitmapFontFace(FTFontData font_data, int size, std::string characters) {
	if (auto end_it = utf8::find_invalid(characters.begin(), characters.end());
		end_it != characters.end()) {
		return Error(std::make_unique<AssetsError>(
			"Font atlas charset not a valid UTF-8 string"));
	}
	if (auto err = Growl::Internal::setFontFacePixelSize(font_data, size);
		err) {
		return std::move(err);
	}

	std::vector<stbrp_rect> glyph_rects;
	if (characters.empty()) {
		for (int i = 0; i < font_data.face->num_glyphs; i++) {
			if (auto err = FT_Load_Glyph(
					font_data.face, i, FT_LOAD_BITMAP_METRICS_ONLY);
				err) {
				return Error(
					std::make_unique<FontError>("Failed to load glyph", err));
			}
			auto& metrics = font_data.face->glyph->metrics;
			// Glyph sizes are represented in 26.6 fractional format, so we
			// shift them to get the pixel sizes.
			glyph_rects.push_back(stbrp_rect{
				i, static_cast<stbrp_coord>(metrics.width >> 6) + SPACING * 2,
				static_cast<stbrp_coord>(metrics.height >> 6) + SPACING * 2});
		}
	} else {
		// If characters is specified, use a Harfbuzz buffer to work
		// out what we actually need to create.
		hb_buffer_t* buffer = hb_buffer_create();
		hb_font_t* face = hb_ft_font_create(font_data.face, 0);
		hb_buffer_add_utf8(buffer, characters.c_str(), -1, 0, -1);
		hb_buffer_guess_segment_properties(buffer);
		hb_shape(face, buffer, 0, 0);
		unsigned int len_paragraph = hb_buffer_get_length(buffer);
		hb_glyph_info_t* info_paragraph = hb_buffer_get_glyph_infos(buffer, 0);
		std::set<int> glyphs_seen;

		for (unsigned int i = 0; i < len_paragraph; i++) {
			hb_codepoint_t glyph = info_paragraph[i].codepoint;
			if (auto [_, inserted] = glyphs_seen.insert(glyph); !inserted) {
				// Already seen glyph
				continue;
			}
			if (auto err = FT_Load_Glyph(
					font_data.face, glyph, FT_LOAD_BITMAP_METRICS_ONLY);
				err) {
				return Error(
					std::make_unique<FontError>("Failed to load glyph", err));
			}
			auto& metrics = font_data.face->glyph->metrics;
			// Glyph sizes are represented in 26.6 fractional format, so we
			// shift them to get the pixel sizes.
			glyph_rects.push_back(stbrp_rect{
				static_cast<int>(glyph),
				static_cast<stbrp_coord>(metrics.width >> 6) + SPACING * 2,
				static_cast<stbrp_coord>(metrics.height >> 6) + SPACING * 2});
		}
		hb_font_destroy(face);
		hb_buffer_destroy(buffer);
	}

	int width, height;
	if (!Growl::Internal::packRectsIncreasing(
			glyph_rects,
			Growl::Internal::nextPowerOfTwo(
				std::max(glyph_rects[0].w, glyph_rects[0].h)),
			&width, &height)) {
		return Error(std::make_unique<AssetsError>(
			"Failed to pack font in texture; too large"));
	}

	std::unordered_map<int, AtlasRegion> glyphs;
	std::vector<unsigned char> image_data(width * height * sizeof(uint32_t), 0);
	const float inv_tex_width = 1.0f / width;
	const float inv_tex_height = 1.0f / height;
	bool has_color = FT_HAS_COLOR(font_data.face);
	int load_params = FT_LOAD_RENDER;
	if (has_color) {
		load_params |= FT_LOAD_COLOR;
	}
	for (auto& rect : glyph_rects) {
		if (auto err = FT_Load_Glyph(font_data.face, rect.id, load_params);
			err) {
			return Error(std::make_unique<FontError>(
				"Failed to load glyph bitmap", err));
		}
		auto& bitmap = font_data.face->glyph->bitmap;
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

		glyphs[rect.id] = AtlasRegion{
			(rect.x + SPACING) * inv_tex_width,
			(rect.y + SPACING) * inv_tex_height,
			(rect.x + (rect.w - SPACING)) * inv_tex_width,
			(rect.y + (rect.h - SPACING)) * inv_tex_height};
	}

	return FontFace(
		FontFaceType::Bitmap, std::move(font_data),
		std::make_unique<Image>(width, height, 4, image_data),
		std::move(glyphs));
}
