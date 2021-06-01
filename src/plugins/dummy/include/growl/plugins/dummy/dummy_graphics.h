#pragma once

#include <growl/core/api/api.h>

namespace Growl {

class DummyGraphicsAPI : public GraphicsAPIInternal {
public:
	void create() override;
	void dispose() override;
	void clear(float r, float g, float b) override;
};

} // namespace Growl
