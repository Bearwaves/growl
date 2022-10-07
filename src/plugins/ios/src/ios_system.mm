#include "ios_system.h"
#include "growl/core/error.h"
#include "ios_window.h"
#include <os/log.h>

using Growl::IOSSystemAPI;
using Growl::Error;
using Growl::Window;
using Growl::Result;
using Growl::InputTouchEvent;

Error IOSSystemAPI::init() {
	return nullptr;
}

void IOSSystemAPI::tick() {}

void IOSSystemAPI::dispose() {}

void IOSSystemAPI::onTouch(InputTouchEvent event) {
	if (inputProcessor) {
		inputProcessor->onTouchEvent(event);
	}
}

Result<std::unique_ptr<Window>>
IOSSystemAPI::createWindow(const WindowConfig& config) {
	UIWindow* w = [[[UIApplication sharedApplication] delegate] window];
	return std::unique_ptr<Window>(std::make_unique<IOSWindow>(w));
}

void IOSSystemAPI::setLogLevel(LogLevel log_level) {}

void IOSSystemAPI::logInternal(
	LogLevel log_level, std::string tag, std::string msg) {
	os_log_with_type(
		OS_LOG_DEFAULT, OS_LOG_TYPE_DEBUG, "[%s] %s", tag.c_str(), msg.c_str());
}

std::string IOSSystemAPI::getResourcePath(std::string path) {
	return [[NSBundle mainBundle]
			   pathForResource:[NSString stringWithUTF8String:path.c_str()]
						ofType:@""]
		.UTF8String;
}
