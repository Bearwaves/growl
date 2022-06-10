#pragma once

#include "../../../../thirdparty/harfbuzz/src/hb.h"
#include "../../../thirdparty/stb_rect_pack/stb_rect_pack.h"
#include "freetype/freetype.h"
#include "growl/util/text/glyph_layout.h"
#include <vector>

class Growl::FTFontData {
public:
	FT_Library library;
	FT_Face face;
	std::vector<unsigned char> data;

	FTFontData(FT_Library library, FT_Face face)
		: library{library}
		, face{face} {}
	FTFontData(
		FT_Library library, FT_Face face, std::vector<unsigned char>&& data)
		: library{library}
		, face{face}
		, data{std::move(data)} {}

	// FTFontData is move-only
	FTFontData(const FTFontData&) = delete;
	FTFontData& operator=(const FTFontData&) = delete;
	FTFontData(FTFontData&& f) {
		this->library = f.library;
		this->face = f.face;
		this->data = std::move(f.data);
		f.library = nullptr;
		f.face = nullptr;
	}
	FTFontData& operator=(FTFontData&& f) {
		this->library = f.library;
		this->face = f.face;
		this->data = std::move(f.data);
		f.library = nullptr;
		f.face = nullptr;
		return *this;
	}

	~FTFontData() {
		if (face) {
			FT_Done_Face(face);
		}
		if (library) {
			FT_Done_FreeType(library);
		}
	}
};

struct Growl::HBData {
	hb_font_t* font;
	hb_buffer_t* buffer_paragraph;
	hb_buffer_t* buffer_line;
};

namespace {
class FontError : public Growl::BaseError {
public:
	FontError(std::string msg, FT_Error ft_err)
		: msg{msg}
		, ft_err{ft_err} {}
	std::string message() override {

		return msg + ": " + FT_Error_String(ft_err);
	}

private:
	std::string msg;
	FT_Error ft_err;
};
} // namespace

namespace Growl::Internal {
Growl::Error
setFontFacePixelSize(Growl::FTFontData& font_data, int size) noexcept;
bool packRectsIncreasing(
	std::vector<stbrp_rect>& rects, int start_size, int* out_width,
	int* out_height);

inline int nextPowerOfTwo(int n) {
	int i = 2;
	while (i < n) {
		i *= 2;
	}
	return i;
}
} // namespace Growl::Internal
