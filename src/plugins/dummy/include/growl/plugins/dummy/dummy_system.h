#pragma once

#include <growl/core/api/api.h>

namespace Growl {

class DummySystemAPI : public SystemAPIInternal {
	void create() override;
	void dispose() override;
};

} // namespace Growl
