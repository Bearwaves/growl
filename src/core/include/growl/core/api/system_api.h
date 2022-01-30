#pragma once

#include <memory>
namespace Growl {

class Window;
class WindowConfig;

class SystemAPI {
public:
	virtual ~SystemAPI() {}
	virtual bool isRunning() = 0;
	virtual void tick() = 0;
	virtual std::shared_ptr<Window>
	createWindow(const WindowConfig& window) = 0;
};

} // namespace Growl
