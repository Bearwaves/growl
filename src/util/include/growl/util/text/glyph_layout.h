#pragma once

#include "growl/util/assets/font_atlas.h"
#include <string>

namespace Growl {

struct HBData;

struct LayoutInfo {
	int glyph_id;
	int x;
	int y;
	int w;
	int h;
};

enum class GlyphLayoutAlignment { Auto, Left, Right, Center };

class GlyphLayout {
public:
	GlyphLayout(
		Font& font, std::string text, int width,
		GlyphLayoutAlignment align = GlyphLayoutAlignment::Auto,
		std::string lang = "en") noexcept;
	~GlyphLayout() noexcept;

	void layout() noexcept;

	void setText(std::string text);

	void setWidth(int width);

	const int getWidth() const {
		return width;
	}

	const int getHeight() const {
		return height;
	}

	const int getOriginOffset() const {
		return origin_offset;
	}

	const std::vector<LayoutInfo>& getLayout() const {
		return layout_info;
	}

	const bool isOverflowed() const {
		return overflowed;
	}

private:
	std::string text;
	std::string lang;
	GlyphLayoutAlignment align;
	int width;
	int height;
	int origin_offset;
	bool overflowed;
	std::unique_ptr<HBData> hb_data;
	std::vector<LayoutInfo> layout_info;
	std::vector<char> breaks;
};

} // namespace Growl
