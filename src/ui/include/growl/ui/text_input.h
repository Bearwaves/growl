#pragma once

#include "growl/core/text/glyph_layout.h"
#include "growl/ui/widget.h"

namespace Growl {

class SystemAPI;

class TextInput : public Widget {
public:
	TextInput(
		std::string&& name, SystemAPI& system, FontTextureAtlas& font_tex,
		FontFace& font, Value font_size, Value max_width = Value(),
		Align align_h = Align::LEFT, Align align_v = Align::MIDDLE);

	void layout() override;

	virtual void
	onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) override;

	virtual bool onKeyboardEvent(const InputKeyboardEvent& event) override;
	virtual bool onMouseEvent(const InputMouseEvent& event) override;
	virtual bool onTouchEvent(const InputTouchEvent& event) override;

	virtual void setText(std::string text);
	std::string getText();

	bool isActive() {
		return active;
	}
	void setActive(bool active);

	void setX(float x) override;
	void setY(float y) override;
	void setWidth(float w) override;
	void setHeight(float h) override;

private:
	SystemAPI& system;
	FontTextureAtlas& font_tex;
	std::unique_ptr<GlyphLayout> glyph_layout;
	Value font_size;
	Value max_width;
	Align align_h;
	Align align_v;
	float pref_width;
	float pref_height;
	float last_font_size;
	bool active = false;
	int cursor;
	bool needs_cursor_update = false;
	float layout_y = 0;
	float layout_x = 0;
	float cursor_y = 0;

	void handlePointer(int x, int y);
};

} // namespace Growl
