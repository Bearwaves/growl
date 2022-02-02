#pragma once

#include <growl/core/api/api.h>
#include <memory>

namespace Growl {

class SDL2SystemAPI : public SystemAPIInternal {
public:
	void init() override;
	void tick() override;
	void dispose() override;
	bool isRunning() override;
	virtual std::unique_ptr<Window>
	createWindow(const WindowConfig& config) override;

private:
	bool running;
};

} // namespace Growl
