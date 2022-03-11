#pragma once

#include "../graphics/batch.h"
#include "../graphics/texture.h"
#include "../graphics/window.h"
#include <growl/util/resources/image.h>
#include <memory>

namespace Growl {

class GraphicsAPI {

public:
	virtual ~GraphicsAPI() {}
	double getDeltaTime() {
		return deltaTime;
	}
	virtual void clear(float r, float g, float b) = 0;
	virtual std::unique_ptr<Texture> createTexture(Image* image) = 0;
	virtual std::unique_ptr<Batch> createBatch() = 0;

protected:
	double deltaTime = 0;
};

} // namespace Growl
