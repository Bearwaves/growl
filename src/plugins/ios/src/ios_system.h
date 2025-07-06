#pragma once

#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "ios_haptics.h"
#include "ios_preferences.h"
#include <GameController/GameController.h>

namespace Growl {

class IOSSystemAPI : public SystemAPIInternal {
public:
	explicit IOSSystemAPI(API& api)
		: api{api} {}
	Error init(const Config& config) override;
	void tick() override;
	void dispose() override;
	void resume() override;
	void onTouch(InputTouchEvent) override;
	bool isRunning() override {
		return running;
	}

	virtual Result<std::unique_ptr<Window>>
	createWindow(const Config& config) override;
	void setLogLevel(LogLevel log_level) override;

	Result<std::unique_ptr<File>> openFile(std::string path) override;

	std::string getPlatformName() override;
	std::string getPlatformOSVersion() override;
	std::string getDeviceManufacturer() override;
	std::string getDeviceModel() override;

	Preferences& getLocalPreferences() override {
		return *local_preferences;
	}

	Preferences& getSharedPreferences() override {
		return *shared_preferences;
	}

	bool hasSharedPreferences() override {
		return has_shared_preferences;
	}

	HapticsDevice* getHaptics() override;

private:
	void
	logInternal(LogLevel log_level, std::string tag, std::string msg) override;
	void openGameController(GCController* controller);
	void closeGameController(GCController* controller);
	void handleControllerInput(
		GCExtendedGamepad* gamepad, GCControllerElement* element);
	void dispatchControllerEvent(
		ControllerButton button, ControllerEventType event_type);
	ControllerEventType controllerEventTypeForButtonPressed(bool pressed);

	API& api;
	bool running;
	GCController* controller = nullptr;
	id game_controller_connect_observer;
	id game_controller_disconnect_observer;
	std::unique_ptr<IOSPreferences> local_preferences;
	std::unique_ptr<IOSPreferences> shared_preferences;
	std::unique_ptr<IOSHapticsDevice> controller_haptics = nullptr;
	std::unique_ptr<IOSHapticsDevice> device_haptics;
	bool has_shared_preferences = false;
};

} // namespace Growl
