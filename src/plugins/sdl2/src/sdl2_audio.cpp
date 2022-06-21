#include "sdl2_audio.h"
#include "SDL.h"
#include "SDL_audio.h"
#include "growl/core/audio/device.h"
#include "growl/util/error.h"
#include "sdl2_error.h"
#include <memory>
#include <vector>

using Growl::Error;
using Growl::SDL2AudioAPI;
using Growl::SDL2Error;

Error SDL2AudioAPI::init() {
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1) {
		return std::make_unique<SDL2Error>(SDL_GetError());
	}

	std::vector<AudioDevice> devices;
	int num_devices = SDL_GetNumAudioDevices(0);
	for (int i = 0; i < num_devices; i++) {
		std::string name = SDL_GetAudioDeviceName(i, 0);
		SDL_AudioSpec spec;
		if (SDL_GetAudioDeviceSpec(i, 0, &spec) != 0) {
			return std::make_unique<SDL2Error>(SDL_GetError());
		}
		devices.push_back(AudioDevice(name, spec.freq));
	}
	this->devices = std::move(devices);

	system.log(
		"SDL2AudioAPI", "Initialised SDL audio; {} devices found", num_devices);
	for (const auto& device : this->devices) {
		system.log(
			"SDL2AudioAPI", "=> Found device [{}] at {}Hz", device.getName(),
			device.getSampleRate());
	}

	return nullptr;
}

void SDL2AudioAPI::dispose() {}
