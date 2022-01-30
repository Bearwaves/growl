#pragma once

#include <growl/core/api/api.h>

namespace Growl {

class SDL2SystemAPI : public SystemAPIInternal {
public:
	void init() override;
	void tick() override;
	void dispose() override;
	bool isRunning() override;

private:
	bool running;
};

} // namespace Growl
