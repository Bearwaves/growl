#include "growl/scene/rectangle.h"
#include "growl/core/graphics/batch.h"

using Growl::Batch;
using Growl::Rectangle;

void Rectangle::onDraw(
	Batch& batch, float parent_alpha, glm::mat4x4 transform) {
	Color c = batch.getColor();
	batch.setColor(color.r, color.g, color.b, color.a * parent_alpha);
	batch.drawRect(0, 0, getWidth(), getHeight(), transform);
	batch.setColor(c);
}
