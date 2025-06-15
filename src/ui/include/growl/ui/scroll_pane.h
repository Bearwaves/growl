#pragma once

#include "growl/ui/widget.h"
namespace Growl {

class ScrollPane : public Widget {
public:
	ScrollPane(std::string&& name)
		: Widget{std::move(name)} {}

	void layout() override;
	void
	onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) override;

	bool onMouseEvent(const InputMouseEvent& event) override;
	bool onTouchEvent(const InputTouchEvent& event) override;

	bool onPostEvent(const InputEvent& event, bool children_handled) override;
	bool onMouseEventPost(const InputMouseEvent& event, bool child_handled);
	bool onTouchEventPost(const InputTouchEvent& event, bool child_handled);

	float getScrollX();
	float getScrollY();
	void setScrollX(float x);
	void setScrollY(float y);

private:
	bool panning = false;
	float last_pointer_x = 0;
	float last_pointer_y = 0;
	float min_x = 0;
	float min_y = 0;
	float max_x = 0;
	float max_y = 0;

	void pan(float x, float y);
	float clampX(float x);
	float clampY(float y);
};

} // namespace Growl
