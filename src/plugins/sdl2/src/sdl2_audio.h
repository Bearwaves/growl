#pragma once

#include "growl/core/api/api_internal.h"
#include "growl/core/audio/device.h"
#include "growl/util/error.h"
#include "sdl2_system.h"
#include <memory>
#include <string>
#include <vector>
namespace Growl {

class SDL2AudioAPI : public AudioAPIInternal {
public:
	explicit SDL2AudioAPI(SystemAPI& system)
		: system{system} {}
	Error init() override;
	void dispose() override;
	const std::vector<AudioDevice>& getDevices() override {
		return devices;
	}

private:
	SystemAPI& system;
	std::vector<AudioDevice> devices;
};

} // namespace Growl
