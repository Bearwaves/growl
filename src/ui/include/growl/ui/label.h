#pragma once

#include "growl/core/text/glyph_layout.h"
#include "growl/ui/widget.h"

namespace Growl {

class Label : public Widget {
public:
	Label(
		std::string&& name, FontTextureAtlas& font_tex,
		std::unique_ptr<GlyphLayout> glyph_layout);

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
};

} // namespace Growl
