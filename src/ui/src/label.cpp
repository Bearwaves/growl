#include "growl/ui/label.h"
#include "growl/core/graphics/batch.h"
#include "growl/core/text/glyph_layout.h"
#include "growl/ui/widget.h"

using Growl::Label;
using Growl::Widget;

Label::Label(
	std::string&& name, std::string text, FontTextureAtlas& font_tex,
	FontFace& font, Value font_size, bool wrap, Value max_width)
	: Widget{std::move(name)}
	, font_tex{font_tex}
	, glyph_layout{std::make_unique<GlyphLayout>(font, std::move(text), 0, 0)}
	, font_size{font_size}
	, wrap{wrap}
	, max_width{max_width} {}

void Label::onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) {
	Widget::onDraw(batch, parent_alpha, transform);
	Color c = batch.getColor();
	batch.setColor(c.multiplyAlpha(parent_alpha));
	batch.draw(*glyph_layout, font_tex, 0, 0, transform);
	batch.setColor(c);
}

void Label::layout() {
	bool should_invalidate = false;
	float font_size_val = font_size.evaluate(this);
	glyph_layout->setFontSize(font_size_val);

	// TODO rethink how maxWidth vs width is used
	if (wrap) {
		glyph_layout->setWidth(getWidth());
	}
	if (max_width && glyph_layout->getWidth() > max_width.evaluate(this)) {
		font_size_val = glyph_layout->getFontSize() * max_width.evaluate(this) /
						glyph_layout->getWidth();
		glyph_layout->setFontSize(font_size_val);
	}

	if (font_size_val != last_font_size) {
		should_invalidate = true;
		last_font_size = font_size_val;
	}

	if (glyph_layout->getWidth() != pref_width) {
		pref_width = glyph_layout->getWidth();
		should_invalidate = true;
	}

	// TODO should expose line height
	float height = glyph_layout->getHeight();
	if (height != pref_height) {
		pref_height = height;
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
