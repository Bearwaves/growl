#pragma once

#include "growl/core/haptics.h"
#include <CoreHaptics/CoreHaptics.h>
#include <GameController/GameController.h>

namespace Growl {

class SystemAPI;

class IOSHapticsDevice : public HapticsDevice {
public:
	explicit IOSHapticsDevice(
		SystemAPI& system, GCController* controller = nullptr);
	~IOSHapticsDevice();

	bool supportsEventType(HapticsEventType type) override;

	Error playEvent(HapticsEvent event, float intensity) override;
	Error
	playPattern(std::vector<HapticsEvent> pattern, float intensity) override;

	void setGameController(GCController* controller);

	void restart();

private:
	SystemAPI& system;
	GCController* controller = nullptr;
	CHHapticEngine* left_handle = nullptr;
	CHHapticEngine* right_handle = nullptr;
	CHHapticEngine* left_trigger = nullptr;
	CHHapticEngine* right_trigger = nullptr;
	CHHapticEngine* device = nullptr;

	void stopAllEngines();
	Error doRumbleEvent(
		HapticsEvent& event, CHHapticEngine* left, CHHapticEngine* right,
		float intensity);
	Result<CHHapticPattern*>
	createRumblePattern(HapticsEvent& event, int loc, float intensity);
};

} // namespace Growl
