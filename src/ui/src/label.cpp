#include "growl/ui/label.h"
#include "growl/core/graphics/batch.h"
#include "growl/ui/widget.h"

using Growl::Label;
using Growl::Widget;

Label::Label(
	std::string&& name, FontTextureAtlas& font_tex,
	std::unique_ptr<GlyphLayout> glyph_layout, bool wrap)
	: Widget{std::move(name)}
	, font_tex{font_tex}
	, glyph_layout{std::move(glyph_layout)}
	, wrap{wrap} {}

void Label::onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) {
	Widget::onDraw(batch, parent_alpha, transform);
	batch.draw(*glyph_layout, font_tex, 0, 0, transform);
}

void Label::layout() {
	bool should_invalidate = false;
	if (wrap) {
		glyph_layout->setWidth(getWidth());
	}
	if (glyph_layout->getWidth() != getWidthRaw()) {
		setWidth(glyph_layout->getWidth());
		should_invalidate = true;
	}
	if (glyph_layout->getHeight() != getHeightRaw()) {
		setHeight(glyph_layout->getHeight());
		should_invalidate = true;
	}

	if (should_invalidate) {
		invalidateHierarchy();
	}
}

void Label::setText(std::string text) {
	glyph_layout->setText(text);
	invalidate();
}
