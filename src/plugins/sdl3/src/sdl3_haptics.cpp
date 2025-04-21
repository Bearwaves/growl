#include "sdl3_haptics.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_gamepad.h"
#include "growl/core/api/system_api.h"
#include "growl/core/error.h"
#include "growl/core/haptics.h"
#include "sdl3_error.h"

using Growl::Error;
using Growl::HapticsDevice;
using Growl::HapticsEvent;
using Growl::SDL3HapticsDevice;
using Growl::SystemAPI;

SDL3HapticsDevice::SDL3HapticsDevice(SystemAPI* system, SDL_Gamepad* gamepad)
	: gamepad{gamepad} {
	auto properties = SDL_GetGamepadProperties(gamepad);
	if (SDL_GetBooleanProperty(
			properties, SDL_PROP_GAMEPAD_CAP_RUMBLE_BOOLEAN, false)) {
		rumble = true;
		system->log("SDL3HapticsDevice", "Controller supports rumble");
	}
	if (SDL_GetBooleanProperty(
			properties, SDL_PROP_GAMEPAD_CAP_TRIGGER_RUMBLE_BOOLEAN, false)) {
		trigger_rumble = true;
		system->log("SDL3HapticsDevice", "Controller supports trigger rumble");
	}
}

bool SDL3HapticsDevice::supportsEventType(HapticsEventType type) {
	switch (type) {
	case HapticsEventType::Rumble:
		return rumble;
	case HapticsEventType::TriggerRumble:
		return trigger_rumble;
	default:
		break;
	}
	return false;
}

Error SDL3HapticsDevice::playEvent(HapticsEvent event) {
	switch (event.type) {
	case HapticsEventType::Rumble:
		if (!(SDL_RumbleGamepad(
				gamepad, 0xFFFF * event.intensity[0],
				0xFFFF * event.intensity[1], event.duration * 1000))) {
			return Error(
				std::make_unique<SDL3Error>(
					std::string("Failed to rumble: ") + SDL_GetError()));
		}
		return nullptr;
	case HapticsEventType::TriggerRumble:
		if (!(SDL_RumbleGamepadTriggers(
				gamepad, 0xFFFF * event.intensity[0],
				0xFFFF * event.intensity[1], event.duration * 1000))) {
			return Error(
				std::make_unique<SDL3Error>(
					std::string("Failed to rumble triggers: ") +
					SDL_GetError()));
		}
		return nullptr;
	case HapticsEventType::Pattern:
		break;
	}
	return nullptr;
}

Error SDL3HapticsDevice::playPattern(std::vector<HapticsEvent> pattern) {
	// Haptic patterns not supported on SDL3.
	return nullptr;
}
