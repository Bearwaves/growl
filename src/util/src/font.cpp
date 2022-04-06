#include <growl/util/assets/font.h>

#include <freetype/freetype.h>
#include <freetype/ftmodapi.h>
#include <growl/util/assets/error.h>
#include <iostream>
#include <memory>

using Growl::Error;
using Growl::Font;
using Growl::Result;

struct Growl::FTFontData {
	FT_Library library;
	FT_Face face;
};

Font::Font(Growl::FTFontData ft_data)
	: ft_data{std::make_unique<Growl::FTFontData>(std::move(ft_data))} {}

Font::~Font() {
	if (ft_data) {
		FT_Done_Face(ft_data->face);
		FT_Done_FreeType(ft_data->library);
	}
}

Result<Font> Growl::loadFontFromFile(std::string filepath) {
	FT_Library lib;
	FT_Face face;

	if (auto err = FT_Init_FreeType(&lib); err) {
		return Error(std::make_unique<AssetsError>("Failed to init FreeType"));
	}

	if (auto err = FT_New_Face(lib, filepath.c_str(), 0, &face); err) {
		FT_Done_Library(lib);
		return Error(std::make_unique<AssetsError>("Failed to load font file"));
	}

	return Font(FTFontData{lib, face});
}

/*Font::Font() {
	hb_buffer_t* buf = hb_buffer_create();
	hb_buffer_add_utf8(buf, "some text", -1, 0, -1);
	hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
	  hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
	  hb_buffer_set_language(buf, hb_language_from_string("en", -1));

			hb_blob_t *blob = hb_blob_create_from_file("../assets/andada.otf");
	  hb_face_t *face = hb_face_create(blob, 0);
	  hb_font_t *font = hb_font_create(face);

			hb_shape(font, buf, nullptr, 0);

			unsigned int glyph_count;
	  hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf,
&glyph_count); hb_glyph_position_t *glyph_pos =
hb_buffer_get_glyph_positions(buf, &glyph_count); std::cout << glyph_count << "
glyphs" << std::endl;

			for (unsigned int i = 0; i < glyph_count; i++) {
				std::cout << glyph_info[i].codepoint << std::endl;
			}

			init_linebreak();
			utf8_t blah[] = "The quick‑brown fox jumps over the lazy dog! !";
			for (auto& c : blah ) {
				std::cout << c;
			}
			std::cout << std::endl;
			char brks[sizeof(blah)];
			set_linebreaks_utf8(blah, sizeof(blah), "en", brks);
			for (size_t i = 0; i < sizeof(blah); i++) {
				std::cout << blah[i] << (brks[i] == LINEBREAK_ALLOWBREAK ? "\n"
: "");
			}
			std::cout << std::endl;

			FT_Library ft_library;
			FT_Face ft_face;
			FT_Error ft_error;
			if ((ft_error = FT_Init_FreeType (&ft_library)))
	abort();
  if (ft_error = FT_New_Face (ft_library, "../assets/andada.otf", 0, &ft_face);
ft_error) { printf("%02x\n", ft_error);
	}
  if ((ft_error = FT_Set_Char_Size (ft_face, 64, 64, 0, 0)))
	abort();
}*/
