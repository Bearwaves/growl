#pragma once

#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "ios_haptics.h"
#include "ios_preferences.h"
#include <GameController/GameController.h>

namespace Growl {

class IOSController : public Controller {
public:
	IOSController(SystemAPI& system, GCController* controller, int id);
	~IOSController();
	HapticsDevice* getHaptics() override;
	int getId() override {
		return id;
	}
	void resumeHaptics();
	GCController* getNative() {
		return controller;
	}

private:
	GCController* controller;
	int id;
	std::unique_ptr<IOSHapticsDevice> haptics;
};

class IOSSystemAPI : public SystemAPIInternal {
public:
	explicit IOSSystemAPI(UITextField* text_field)
		: text_field{text_field} {}
	Error init(const Config& config) override;
	void tick() override;
	void dispose() override;
	void resume() override;
	void onEvent(const InputEvent&) override;
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

	virtual void openURL(std::string url) override;

	virtual void startTextInput(std::string current_text) override;
	virtual void updateTextInput(
		std::string text, int x, int y, int w, int h, int cursor_x) override;
	virtual void stopTextInput() override;

	virtual void shareImage(
		Image& image, std::string title, std::string message,
		Rect rect) override;

	virtual void setStatusBarVisible(bool visible) override;

	bool isStatusBarVisible() override {
		return status_bar_visible;
	}

	bool isControllerConnected() override;

	std::map<int, std::unique_ptr<Controller>>& getControllers() override {
		return controllers;
	}

private:
	void
	logInternal(LogLevel log_level, std::string tag, std::string msg) override;
	void openGameController(GCController* controller);
	void closeGameController(GCController* controller);
	void handleControllerInput(
		GCExtendedGamepad* gamepad, GCControllerElement* element, int id);
	void dispatchControllerButtonEvent(
		ControllerButton button, ControllerEventType event_type, int id);
	void dispatchControllerAxisEvent(ControllerAxis axis, float value, int id);
	ControllerEventType controllerEventTypeForButtonPressed(bool pressed);

	UITextField* text_field;
	bool running;
	int last_controller_id = 0;
	id game_controller_connect_observer;
	id game_controller_disconnect_observer;
	std::unique_ptr<IOSPreferences> local_preferences;
	std::unique_ptr<IOSPreferences> shared_preferences;
	std::unique_ptr<IOSHapticsDevice> device_haptics;
	std::map<int, std::unique_ptr<Controller>> controllers;
	bool has_shared_preferences = false;
	bool status_bar_visible = false;
};

} // namespace Growl
