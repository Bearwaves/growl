#include "font_internal.h"
#include "growl/core/assets/atlas.h"
#include "growl/core/assets/error.h"
#include "growl/core/assets/font_face.h"
#include "growl/core/assets/image.h"
#include "growl/core/error.h"
// Must go after FreeType.
#include "hb-ft.h"
#include "msdfgen-ext.h"
#include "msdfgen.h"
#include <set>

using Growl::AssetsError;
using Growl::AtlasRegion;
using Growl::Error;
using Growl::FontFace;
using Growl::FontFaceType;
using Growl::FTFontData;
using Growl::Image;
using Growl::Result;

constexpr float RANGE = 4.f;
constexpr int BORDER = 2;
constexpr int FT_LOAD_PARAMS = FT_LOAD_RENDER;

Result<FontFace> createDistanceFieldFontFace(
	FTFontData font_data, int size, std::string characters) {
	if (FT_HAS_COLOR(font_data.face)) {
		return Error(std::make_unique<AssetsError>(
			"Failed to create distance field font atlas: cannot create from "
			"coloured font."));
	}
	if (auto err = Growl::Internal::setFontFacePixelSize(font_data, size);
		err) {
		return std::move(err);
	}
	msdfgen::FontHandle* font_handle =
		msdfgen::adoptFreetypeFont(font_data.face);

	std::vector<stbrp_rect> glyph_rects;

	if (characters.empty()) {
		for (int i = 0; i < font_data.face->num_glyphs; i++) {
			if (auto err = FT_Load_Glyph(font_data.face, i, FT_LOAD_PARAMS);
				err) {
				return Error(std::make_unique<FontError>(
					"Failed to load glyph bitmap", err));
			}
			glyph_rects.push_back(stbrp_rect{
				i,
				static_cast<stbrp_coord>(
					font_data.face->glyph->bitmap.width + 2 * BORDER),
				static_cast<stbrp_coord>(
					font_data.face->glyph->bitmap.rows + 2 * BORDER)});
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

			if (auto err = FT_Load_Glyph(font_data.face, glyph, FT_LOAD_PARAMS);
				err) {
				return Error(std::make_unique<FontError>(
					"Failed to load glyph bitmap", err));
			}
			glyph_rects.push_back(stbrp_rect{
				static_cast<int>(glyph),
				static_cast<stbrp_coord>(
					font_data.face->glyph->bitmap.width + 2 * BORDER),
				static_cast<stbrp_coord>(
					font_data.face->glyph->bitmap.rows + 2 * BORDER)});
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
	const float inv_tex_width = 1.0f / width;
	const float inv_tex_height = 1.0f / height;

	std::unordered_map<int, AtlasRegion> glyphs;
	std::vector<unsigned char> image_data(width * height * 4, 0);
	for (const auto& rect : glyph_rects) {
		msdfgen::Shape shape;
		msdfgen::loadGlyph(
			shape, font_handle, msdfgen::GlyphIndex(rect.id),
			msdfgen::FONT_SCALING_EM_NORMALIZED);
		if (shape.validate() && shape.contours.size() > 0) {
			shape.inverseYAxis = true;
			shape.normalize();
			shape.orientContours();
			msdfgen::edgeColoringSimple(shape, 3);

			int load_params = FT_LOAD_RENDER;
			if (auto err = FT_Load_Glyph(font_data.face, rect.id, load_params);
				err) {
				return Error(std::make_unique<FontError>(
					"Failed to load glyph bitmap", err));
			}

			float bl =
				(font_data.face->glyph->bitmap_left - BORDER) / (float)size;
			float bb = (rect.h - font_data.face->glyph->bitmap_top - BORDER) /
					   (float)size;

			msdfgen::Bitmap<float, 4> bitmap(rect.w, rect.h);
			msdfgen::SDFTransformation t(
				msdfgen::Projection(size, msdfgen::Vector2(-bl, bb)),
				msdfgen::Range(RANGE / size));
			msdfgen::generateMTSDF(bitmap, shape, t);

			for (int row = 0; row < bitmap.height(); row++) {
				for (int col = 0; col < bitmap.width(); col++) {
					int x = rect.x + col;
					int y = rect.y + row;
					unsigned char* dst =
						image_data.data() + 4 * (y * width + x);
					*dst++ = msdfgen::pixelFloatToByte(bitmap(col, row)[0]);
					*dst++ = msdfgen::pixelFloatToByte(bitmap(col, row)[1]);
					*dst++ = msdfgen::pixelFloatToByte(bitmap(col, row)[2]);
					*dst++ = msdfgen::pixelFloatToByte(bitmap(col, row)[3]);
				}
			}

			glyphs[rect.id] = AtlasRegion{
				bitmap.width(),
				bitmap.height(),
				(rect.x + BORDER - 0.5f) * inv_tex_width,
				(rect.y + BORDER - 0.5f) * inv_tex_height,
				(rect.x + rect.w - BORDER + 0.5f) * inv_tex_width,
				(rect.y + rect.h - BORDER + 0.5f) * inv_tex_height};
		}
	}

	msdfgen::destroyFont(font_handle);
	return FontFace(
		FontFaceType::MSDF, std::move(font_data),
		std::make_unique<Image>(width, height, 4, image_data),
		std::move(glyphs), RANGE);
}
