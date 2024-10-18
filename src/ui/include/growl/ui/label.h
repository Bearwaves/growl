#pragma once

#include "growl/core/text/glyph_layout.h"
#include "growl/ui/widget.h"

namespace Growl {

class Label : public Widget {
public:
	Label(
		std::string&& name, std::string text, FontTextureAtlas& font_tex,
		FontFace& font, Value font_size, bool wrap = false);

	void layout() override;

	virtual void
	onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) override;

	GlyphLayout& getGlyphLayout() {
		return *glyph_layout;
	}

	void setText(std::string text);

private:
	FontTextureAtlas& font_tex;
	std::unique_ptr<GlyphLayout> glyph_layout;
	Value font_size;
	bool wrap;
};

} // namespace Growl
