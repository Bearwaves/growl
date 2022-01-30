#pragma once

#include <growl/core/api/api.h>
#include <growl/core/api/system_api.h>

namespace Growl {

class MetalGraphicsAPI : public GraphicsAPIInternal {
public:
	explicit MetalGraphicsAPI(SystemAPI& system);
	void init() override;
	void dispose() override;
	void setWindow(WindowConfig& windowConfig) override;
	void clear(float r, float g, float b) override;

private:
	SystemAPI& system;
	std::shared_ptr<Window> window;
};

} // namespace Growl
