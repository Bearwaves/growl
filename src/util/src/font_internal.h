#include <freetype/freetype.h>
#include <growl/util/assets/font.h>

struct Growl::FTFontData {
	FT_Library library;
	FT_Face face;
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