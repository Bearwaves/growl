#pragma once

#include <growl/core/graphics/window.h>

namespace Growl {

class GraphicsAPI {
public:
	virtual ~GraphicsAPI() {}
	virtual void clear(float r, float g, float b) = 0;
};

} // namespace Growl
