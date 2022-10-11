#pragma once

#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include <GameController/GameController.h>

namespace Growl {

class IOSSystemAPI : public SystemAPIInternal {
public:
	explicit IOSSystemAPI(API& api)
		: api{api} {}
	Error init() override;
	void tick() override;
	void dispose() override;
	void onTouch(InputTouchEvent) override;
	bool isRunning() override {
		return running;
	}
	virtual Result<std::unique_ptr<Window>>
	createWindow(const WindowConfig& config) override;
	void setLogLevel(LogLevel log_level) override;
	Result<std::unique_ptr<File>>
	openFile(std::string path, size_t start = 0, size_t end = 0) override;

private:
	void
	logInternal(LogLevel log_level, std::string tag, std::string msg) override;
	void openGameController(GCController* controller);
	void closeGameController(GCController* controller);
	void handleControllerInput(
		GCExtendedGamepad* gamepad, GCControllerElement* element);
	void dispatchControllerEvent(
		ControllerButton button, ControllerEventType eventType);
	ControllerEventType controllerEventTypeForButtonPressed(bool pressed);

	API& api;
	bool running;
	GCController* controller = nullptr;
	id game_controller_connect_observer;
	id game_controller_disconnect_observer;
};

} // namespace Growl
