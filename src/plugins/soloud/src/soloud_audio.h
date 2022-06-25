#pragma once

#include "growl/core/api/api_internal.h"
#include "soloud.h"
#include <memory>
namespace Growl {

class SoLoudAudioAPI : public AudioAPIInternal {
public:
	explicit SoLoudAudioAPI(SystemAPI& system)
		: system{system} {}
	Error init() override;
	void dispose() override;
	const std::vector<AudioDevice>& getDevices() override {
		return devices;
	}

private:
	SystemAPI& system;
	std::unique_ptr<SoLoud::Soloud> soloud;
	std::vector<AudioDevice> devices;
};

}; // namespace Growl
