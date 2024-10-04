#include "growl/ui/label.h"
#include "growl/core/graphics/batch.h"
#include "growl/ui/widget.h"

using Growl::Label;
using Growl::Widget;

Label::Label(
	std::string&& name, FontTextureAtlas& font_tex,
	std::unique_ptr<GlyphLayout> glyph_layout)
	: Widget{std::move(name)}
	, font_tex{font_tex}
	, glyph_layout{std::move(glyph_layout)} {}

void Label::onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) {
	Widget::onDraw(batch, parent_alpha, transform);
	batch.draw(*glyph_layout, font_tex, 0, 0, transform);
}

void Label::layout() {
	glyph_layout->setWidth(getWidth());
	if (getHeightRaw() != glyph_layout->getHeight()) {
		setHeight(glyph_layout->getHeight());
		invalidateHierarchy();
	}
}

void Label::setText(std::string text) {
	glyph_layout->setText(text);
	invalidate();
}
