#include "growl/ui/text_input.h"
#include "growl/core/graphics/batch.h"
#include "growl/core/input/event.h"
#include "growl/core/input/keyboard.h"
#include "growl/ui/widget.h"

using Growl::Batch;
using Growl::Key;
using Growl::KeyEventType;
using Growl::TextInput;
using Growl::Widget;

TextInput::TextInput(
	std::string&& name, SystemAPI& system, FontTextureAtlas& font_tex,
	FontFace& font, Value font_size, Value max_width)
	: Widget{std::move(name)}
	, system{system}
	, font_tex{font_tex}
	, glyph_layout{std::make_unique<GlyphLayout>(font, "", 0, 0)}
	, font_size{font_size}
	, max_width{max_width} {
	cursor = glyph_layout->getGraphemeCount();
}

void TextInput::layout() {
	bool should_invalidate = false;
	float font_size_val = font_size.evaluate(this);
	glyph_layout->setFontSize(font_size_val);

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
		needs_cursor_update = true;
		invalidateHierarchy();
	}
}

void TextInput::onDraw(
	Batch& batch, float parent_alpha, glm::mat4x4 transform) {
	Widget::onDraw(batch, parent_alpha, transform);
	Color c = batch.getColor();
	batch.setColor(c.multiplyAlpha(parent_alpha));
	batch.draw(
		*glyph_layout, font_tex, 0,
		glyph_layout->getFontSize() - glyph_layout->getOriginOffset(),
		transform);

	if (active) {
		int cursor_x =
			cursor == 0 ? 0 : glyph_layout->getGraphemes().at(cursor - 1).x;
		batch.drawRect(
			cursor_x,
			glyph_layout->getFontSize() - glyph_layout->getOriginOffset(), 3,
			glyph_layout->getFontSize(), transform);

		if (needs_cursor_update) {
			auto world_coords = localToWorldCoordinates(0, 0);
			system.updateTextInput(
				getText(), world_coords.x, world_coords.y, getWidth(),
				getHeight(), cursor_x);
			needs_cursor_update = false;
		}
	}

	batch.setColor(c);
}

bool TextInput::onKeyboardEvent(const InputKeyboardEvent& event) {
	if (!active) {
		return false;
	}
	auto current = getText();
	auto& graphemes = glyph_layout->getGraphemes();
	switch (event.type) {
	case KeyEventType::KeyDown:
		switch (event.key) {
		case Key::Backspace: {
			if (cursor == 0) {
				return true;
			}
			cursor -= 1;
			int retain_point = graphemes.at(cursor).char_idx;
			if (cursor == 0) {
				current = current.substr(retain_point + 1, std::string::npos);
			} else {
				int deletion_point = graphemes.at(cursor - 1).char_idx;
				current = current.substr(0, deletion_point + 1) +
						  current.substr(retain_point + 1, std::string::npos);
			}
			setText(current);
			needs_cursor_update = true;
			return true;
		}
		case Key::ArrowLeft: {
			cursor = std::max(0, cursor - 1);
			needs_cursor_update = true;
			return true;
		}
		case Key::ArrowRight: {
			cursor = std::min(glyph_layout->getGraphemeCount(), cursor + 1);
			needs_cursor_update = true;
			return true;
		}
		default:
			return false;
		}
		break;
	case KeyEventType::TextInput: {
		if (cursor == 0) {
			current = event.text + current;
		} else {
			int insert_point = graphemes.at(cursor - 1).char_idx;
			current = current.substr(0, insert_point + 1) + event.text +
					  current.substr(insert_point + 1, std::string::npos);
		}
		int count = glyph_layout->getGraphemeCount();
		setText(current);
		cursor += glyph_layout->getGraphemeCount() - count;
		needs_cursor_update = true;
		return true;
	}
	default:
		return false;
	}
	return false;
}

bool TextInput::onMouseEvent(const InputMouseEvent& event) {
	if (event.type != PointerEventType::Down) {
		return false;
	}
	if (!hit(event.mouseX, event.mouseY)) {
		setActive(false);
		return false;
	}
	setActive(true);

	handlePointer(event.mouseX, event.mouseY);
	return true;
}

bool TextInput::onTouchEvent(const InputTouchEvent& event) {
	if (event.type != PointerEventType::Down) {
		return false;
	}
	if (!hit(event.touchX, event.touchY)) {
		setActive(false);
		return false;
	}
	setActive(true);

	handlePointer(event.touchX, event.touchY);
	return true;
}

void TextInput::setText(std::string text) {
	if (text == glyph_layout->getText()) {
		return;
	}
	glyph_layout->setText(text);
	invalidateHierarchy();
}

std::string TextInput::getText() {
	return glyph_layout->getText();
}

void TextInput::setActive(bool active) {
	if (active == this->active) {
		return;
	}
	this->active = active;
	this->active ? system.startTextInput(getText()) : system.stopTextInput();
}

void TextInput::setX(float x) {
	Widget::setX(x);
	needs_cursor_update = true;
}

void TextInput::setY(float y) {
	Widget::setY(y);
	needs_cursor_update = true;
}

void TextInput::setWidth(float w) {
	Widget::setWidth(w);
	needs_cursor_update = true;
}

void TextInput::setHeight(float h) {
	Widget::setHeight(h);
	needs_cursor_update = true;
}

void TextInput::handlePointer(int x, int y) {
	glm::vec4 local = worldToLocalCoordinates(x, y);
	int i = -1;
	if (glyph_layout->getGraphemeCount() > 0) {
		if (local.x < glyph_layout->getGraphemes().at(0).x / 2) {
			cursor = 0;
			needs_cursor_update = true;
			return;
		}
		for (i = 0; i < glyph_layout->getGraphemeCount() - 1; i++) {
			float next_x = glyph_layout->getGraphemes().at(i + 1).x;
			if (local.x > next_x) {
				continue;
			}
			float this_x = glyph_layout->getGraphemes().at(i).x;
			if (local.x - this_x < next_x - local.x) {
				break;
			}
		}
	}
	cursor = i + 1;
	needs_cursor_update = true;
	return;
}
