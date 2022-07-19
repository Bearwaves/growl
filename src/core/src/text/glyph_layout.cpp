#include "growl/core/text/glyph_layout.h"
#include "../assets/font_internal.h"
#include "freetype/freetype.h"
#include "growl/core/assets/font_face.h"
#include "hb-ft.h"
#include "linebreak.h"
#include <cctype>

using Growl::FontFace;
using Growl::GlyphLayout;
using Growl::GlyphLayoutAlignment;

GlyphLayout::GlyphLayout(
	FontFace& font, std::string text, int width, int size,
	GlyphLayoutAlignment align, std::string lang) noexcept
	: text{text}
	, lang{lang}
	, align{align}
	, width{width}
	, size{size}
	, requested_width{width}
	, font_face(font)
	, hb_data{std::make_unique<Growl::HBData>(Growl::HBData{
		  hb_ft_font_create(font.getFontData().face, 0), hb_buffer_create(),
		  hb_buffer_create()})} {
	init_linebreak();
	setText(text);
}

GlyphLayout::~GlyphLayout() noexcept {
	hb_font_destroy(hb_data->font);
	hb_buffer_destroy(hb_data->buffer_paragraph);
	hb_buffer_destroy(hb_data->buffer_line);
}

void GlyphLayout::layout() noexcept {
	FT_Face face = font_face.getFontData().face;

	if (size && font_face.getType() != FontFaceType::Bitmap) {
		Growl::Internal::setFontFacePixelSize(font_face.getFontData(), size);
		hb_ft_font_changed(hb_data->font);
	}

	hb_buffer_reset(hb_data->buffer_paragraph);
	hb_buffer_add_utf8(hb_data->buffer_paragraph, text.c_str(), -1, 0, -1);
	hb_buffer_guess_segment_properties(hb_data->buffer_paragraph);
	hb_shape(hb_data->font, hb_data->buffer_paragraph, 0, 0);

	unsigned int len_paragraph =
		hb_buffer_get_length(hb_data->buffer_paragraph);
	hb_glyph_info_t* info_paragraph =
		hb_buffer_get_glyph_infos(hb_data->buffer_paragraph, 0);
	hb_glyph_position_t* pos_paragraph =
		hb_buffer_get_glyph_positions(hb_data->buffer_paragraph, 0);
	bool rtl =
		hb_buffer_get_direction(hb_data->buffer_paragraph) == HB_DIRECTION_RTL;

	overflowed = false;
	int remaining_index = 0;
	int break_index = 0;
	int w = 0;
	int w_break = 0;
	std::vector<std::string> lines;
	for (unsigned int i = 0; i < len_paragraph; i++) {
		int j = rtl ? len_paragraph - (i + 1) : i;
		// On which char does this unicode cluster end? That's where the break
		// info is.
		uint32_t cluster_end =
			i == len_paragraph - 1
				? text.size() - 1
				: info_paragraph[rtl ? j - 1 : j + 1].cluster - 1;
		bool must_break = breaks.at(cluster_end) == LINEBREAK_MUSTBREAK;
		bool allow_break = breaks.at(cluster_end) == LINEBREAK_ALLOWBREAK;
		if (must_break) {
			lines.push_back(
				text.substr(remaining_index, cluster_end - remaining_index));
			w = 0;
			remaining_index = cluster_end + 1;
			continue;
		}
		w += (pos_paragraph[j].x_advance >> 6);
		if (requested_width && w > requested_width) {
			if (remaining_index > break_index) {
				overflowed = true;
				continue;
			}
			lines.push_back(
				text.substr(remaining_index, break_index - remaining_index));
			w -= w_break;
			if (std::isspace(text.at(break_index))) {
				remaining_index = break_index + 1;
			} else {
				remaining_index = break_index;
			}
		}
		if (allow_break) {
			break_index = cluster_end + 1;
			w_break = w;
		}
	}
	lines.push_back(text.substr(remaining_index, text.size()));

	int cursor_x = 0;
	int cursor_y = 0;
	int bearing_up = 0;
	int bearing_down = 0;
	int max_width = 0;
	GlyphLayoutAlignment alignment = align;
	if (alignment == GlyphLayoutAlignment::Auto) {
		alignment =
			rtl ? GlyphLayoutAlignment::Right : GlyphLayoutAlignment::Left;
	}
	std::vector<LayoutInfo> new_layout;
	std::vector<std::pair<int, unsigned int>> line_length_info;

	for (auto& line : lines) {
		hb_buffer_reset(hb_data->buffer_line);
		hb_buffer_add_utf8(hb_data->buffer_line, line.c_str(), -1, 0, -1);
		hb_buffer_guess_segment_properties(hb_data->buffer_line);
		hb_shape(hb_data->font, hb_data->buffer_line, 0, 0);

		unsigned int len = hb_buffer_get_length(hb_data->buffer_line);
		hb_glyph_info_t* info =
			hb_buffer_get_glyph_infos(hb_data->buffer_line, 0);
		hb_glyph_position_t* pos =
			hb_buffer_get_glyph_positions(hb_data->buffer_line, 0);

		int glyphs_added = 0;
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
					cursor_x + (pos[i].x_offset >> 6) +
						face->glyph->bitmap_left,
					cursor_y -
						((pos[i].y_offset >> 6) + face->glyph->bitmap_top),
					static_cast<int>(face->glyph->metrics.width >> 6),
					static_cast<int>(face->glyph->metrics.height >> 6),
				});
				glyphs_added++;
				int y_above_baseline =
					(pos[i].y_offset) + face->glyph->metrics.horiBearingY;
				if (y_above_baseline > bearing_up) {
					bearing_up = y_above_baseline;
				}
				int y_below =
					face->glyph->metrics.height -
					(face->glyph->metrics.horiBearingY + pos[i].y_offset);
				if (y_below > bearing_down) {
					bearing_down = y_below;
				}
			}
			cursor_x += (pos[i].x_advance >> 6);
			cursor_y += (pos[i].y_advance >> 6);
		}

		if (cursor_x > max_width) {
			max_width = cursor_x;
		}

		line_length_info.push_back(
			std::pair<int, unsigned int>{cursor_x, glyphs_added});

		cursor_x = 0;
		cursor_y += (face->size->metrics.height >> 6);
	}

	bearing_up >>= 6;
	bearing_down >>= 6;

	// Align
	int block_width = width > max_width ? width : max_width;
	for (auto& line_length : line_length_info) {
		line_length.first = block_width - line_length.first;
	}
	unsigned int i = 0;
	int line = 0;
	for (auto& glyph : new_layout) {
		while (i >= line_length_info[line].second) {
			i = 0;
			line++;
		}
		glyph.y += bearing_up;
		if (alignment == GlyphLayoutAlignment::Center) {
			glyph.x += line_length_info[line].first / 2;
		} else if (alignment == GlyphLayoutAlignment::Right) {
			glyph.x += line_length_info[line].first;
		}
		i++;
	}

	width = max_width;
	height = (cursor_y - (face->size->metrics.height >> 6)) +
			 (bearing_down + bearing_up);
	origin_offset = bearing_up;
	layout_info = std::move(new_layout);
}

void GlyphLayout::setText(std::string text) {
	this->text = text;
	// Calculate breaks
	std::vector<char> new_breaks(text.size());
	set_linebreaks_utf8(
		reinterpret_cast<const uint8_t*>(text.c_str()), text.size(),
		lang.c_str(), new_breaks.data());
	breaks = std::move(new_breaks);
	layout();
}

void GlyphLayout::setWidth(int width) {
	this->requested_width = width;
	layout();
}

void GlyphLayout::setFontSize(int size) {
	if (font_face.getType() == FontFaceType::Bitmap) {
		return;
	}
	this->size = size;
	layout();
}
