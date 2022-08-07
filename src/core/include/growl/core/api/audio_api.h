#pragma once

#include "growl/core/assets/audio.h"
#include "growl/core/assets/bundle.h"
#include "growl/core/audio/device.h"
#include <memory>
#include <vector>
namespace Growl {

class AudioAPI {
public:
	virtual ~AudioAPI() {}

	virtual const std::vector<AudioDevice>& getDevices() = 0;

	virtual Result<std::unique_ptr<SFX>>
	loadSFXFromBundle(AssetsBundle& bundle, std::string asset) = 0;

	virtual void play(SFX& sfx) = 0;
};

} // namespace Growl
