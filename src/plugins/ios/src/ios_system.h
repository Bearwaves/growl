#pragma once

#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"

namespace Growl {

class IOSSystemAPI : public SystemAPIInternal {
public:
	explicit IOSSystemAPI(API& api)
		: api{api} {}
	Error init() override;
	void tick() override;
	void dispose() override;
	bool isRunning() override {
		return running;
	}
	virtual Result<std::unique_ptr<Window>>
	createWindow(const WindowConfig& config) override;
	void setLogLevel(LogLevel log_level) override;
	std::string getResourcePath(std::string path) override;

private:
	void
	logInternal(LogLevel log_level, std::string tag, std::string msg) override;

	API& api;
	bool running;
};

} // namespace Growl
