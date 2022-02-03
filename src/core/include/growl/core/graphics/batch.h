#pragma once

#include "texture.h"

namespace Growl {

class Batch {
public:
	virtual ~Batch() = default;
	virtual void begin() = 0;
	virtual void end() = 0;
	virtual void
	draw(Texture* texture, float x, float y, float width, float height) = 0;
};

} // namespace Growl
