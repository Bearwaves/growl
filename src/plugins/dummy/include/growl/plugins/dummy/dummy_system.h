#pragma once

#include <growl/core/api/api.h>

namespace Growl {

class DummySystemAPI : public SystemAPIInternal {
public:
	void init() override;
	void dispose() override;
	bool isRunning() override;
};

} // namespace Growl
