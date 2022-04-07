#include <freetype/freetype.h>
#include <growl/util/assets/font.h>

struct Growl::FTFontData {
	FT_Library library;
	FT_Face face;
};
