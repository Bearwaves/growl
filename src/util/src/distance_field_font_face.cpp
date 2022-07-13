#include "font_internal.h"
#include "growl/util/assets/atlas.h"
#include "growl/util/assets/error.h"
#include "growl/util/assets/font_face.h"
#include "growl/util/assets/image.h"
#include "growl/util/error.h"
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
	msdfgen::FontMetrics font_metrics;
	msdfgen::getFontMetrics(font_metrics, font_handle);

	float scale = size / font_metrics.emSize;
	int border = 2;
	int pack_border = 1;
	float range = 2;

	std::vector<stbrp_rect> glyph_rects;
	if (characters.empty()) {
		for (int i = 0; i < font_data.face->num_glyphs; i++) {
			msdfgen::Shape shape;
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

			msdfgen::Shape shape;
			msdfgen::loadGlyph(shape, font_handle, msdfgen::GlyphIndex(glyph));
			if (shape.validate() && shape.contours.size() > 0) {
				shape.inverseYAxis = true;
				shape.normalize();
				shape.orientContours();

				auto bounds = shape.getBounds(border);
				int glyph_width = round((bounds.r - bounds.l) * scale);
				int glyph_height = round((bounds.t - bounds.b) * scale);
				glyph_rects.push_back(stbrp_rect{
					static_cast<int>(glyph), glyph_width + (pack_border * 2),
					glyph_height + (pack_border * 2)});
			}
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
	std::vector<unsigned char> image_data(width * height * 4, 0);
	const float inv_tex_width = 1.0f / width;
	const float inv_tex_height = 1.0f / height;

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

			int border_calc = border * scale;
			glyphs[rect.id] = AtlasRegion{
				(rect.x + pack_border + border_calc) * inv_tex_width,
				(rect.y + pack_border + border_calc) * inv_tex_height,
				(rect.x + (rect.w - (pack_border + border_calc))) *
					inv_tex_width,
				(rect.y + (rect.h - (pack_border + border_calc))) *
					inv_tex_height};
		}
	}

	msdfgen::destroyFont(font_handle);
	return FontFace(
		FontFaceType::MSDF, std::move(font_data),
		std::make_unique<Image>(width, height, 4, image_data),
		std::move(glyphs));
}
