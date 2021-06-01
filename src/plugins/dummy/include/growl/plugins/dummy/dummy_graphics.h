#pragma once

#include <growl/core/api/api.h>

namespace Growl {

class DummyGraphicsAPI : public GraphicsAPIInternal {
public:
	void init() override;
	void dispose() override;
	void clear(float r, float g, float b) override;
};

} // namespace Growl
