#include "growl/util/text/glyph_layout.h"
#include "../../../../thirdparty/harfbuzz/src/hb-ft.h"
#include "font_internal.h"
#include "freetype/freetype.h"

using Growl::GlyphLayout;

GlyphLayout::GlyphLayout(Font& font, std::string text, int width) noexcept
	: text{text}
	, width{width}
	, hb_data{std::make_unique<Growl::HBData>(Growl::HBData{
		  hb_ft_font_create(font.getFTFontData().face, 0),
		  hb_buffer_create()})} {
	layout();
}

GlyphLayout::~GlyphLayout() noexcept {
	hb_font_destroy(hb_data->font);
	hb_buffer_destroy(hb_data->buffer);
}

void GlyphLayout::layout() noexcept {
	hb_buffer_reset(hb_data->buffer);
	hb_buffer_add_utf8(hb_data->buffer, text.c_str(), -1, 0, -1);
	hb_buffer_guess_segment_properties(hb_data->buffer);
	hb_shape(hb_data->font, hb_data->buffer, 0, 0);

	unsigned int len = hb_buffer_get_length(hb_data->buffer);
	hb_glyph_info_t* info = hb_buffer_get_glyph_infos(hb_data->buffer, 0);
	hb_glyph_position_t* pos =
		hb_buffer_get_glyph_positions(hb_data->buffer, 0);

	FT_Face face = hb_ft_font_get_face(hb_data->font);

	std::vector<LayoutInfo> new_layout;
	int cursor_x = 0;
	int cursor_y = 0;
	for (unsigned int i = 0; i < len; i++) {
		int glyph_id = static_cast<int>(info[i].codepoint);
		if (auto err =
				FT_Load_Glyph(face, glyph_id, FT_LOAD_BITMAP_METRICS_ONLY);
			err) {
			return;
		}
		if (face->glyph->metrics.width || face->glyph->metrics.height) {
			new_layout.push_back(LayoutInfo{
				static_cast<int>(info[i].codepoint),
				cursor_x + (pos[i].x_offset >> 6) + face->glyph->bitmap_left,
				cursor_y - ((pos[i].y_offset >> 6) + face->glyph->bitmap_top),
				static_cast<int>(face->glyph->metrics.width >> 6),
				static_cast<int>(face->glyph->metrics.height >> 6),
			});
		}
		cursor_x += (pos[i].x_advance >> 6);
		cursor_y += (pos[i].y_advance >> 6);
	}

	width = cursor_x;
	layout_info = std::move(new_layout);
}

void GlyphLayout::setText(std::string text) {
	this->text = text;
	layout();
}
