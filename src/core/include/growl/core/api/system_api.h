#pragma once

namespace Growl {

class SystemAPI {
public:
	virtual ~SystemAPI() {}
	virtual bool isRunning() = 0;
	virtual void tick() = 0;
};

} // namespace Growl
