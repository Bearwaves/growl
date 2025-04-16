#pragma once

#include "SDL3/SDL_gamepad.h"
#include "growl/core/haptics.h"

namespace Growl {

class SystemAPI;

class SDL3HapticsDevice : public HapticsDevice {
public:
	explicit SDL3HapticsDevice(SystemAPI* system, SDL_Gamepad* gamepad);

	bool supportsEventType(HapticsEventType type) override;
	Error triggerEvent(HapticsEvent event) override;

private:
	SDL_Gamepad* gamepad;
	bool rumble = false;
	bool trigger_rumble = false;
};

} // namespace Growl
