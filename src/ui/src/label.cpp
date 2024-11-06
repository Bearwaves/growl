#include "growl/ui/label.h"
#include "growl/core/graphics/batch.h"
#include "growl/core/text/glyph_layout.h"
#include "growl/ui/widget.h"

using Growl::Label;
using Growl::Widget;

Label::Label(
	std::string&& name, std::string text, FontTextureAtlas& font_tex,
	FontFace& font, Value font_size, bool wrap)
	: Widget{std::move(name)}
	, font_tex{font_tex}
	, glyph_layout{std::make_unique<GlyphLayout>(font, std::move(text), 0, 0)}
	, font_size{font_size}
	, wrap{wrap} {}

void Label::onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) {
	Widget::onDraw(batch, parent_alpha, transform);
	batch.draw(*glyph_layout, font_tex, 0, 0, transform);
}

void Label::layout() {
	bool should_invalidate = false;
	int font_size_val = std::floor(font_size.evaluate(this));
	if (glyph_layout->getFontSize() != font_size_val) {
		glyph_layout->setFontSize(font_size_val);
		should_invalidate = true;
	}
	if (wrap) {
		glyph_layout->setWidth(getWidth());
	}

	if (glyph_layout->getWidth() != pref_width) {
		pref_width = glyph_layout->getWidth();
		should_invalidate = true;
	}
	if (glyph_layout->getHeight() != pref_height) {
		pref_height = glyph_layout->getHeight();
		should_invalidate = true;
	}

	if (should_invalidate) {
		invalidateHierarchy();
	}
}

void Label::setText(std::string text) {
	glyph_layout->setText(text);
	invalidateHierarchy();
}
