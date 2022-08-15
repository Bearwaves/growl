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

	virtual Result<std::unique_ptr<AudioClip>>
	loadClipFromBundle(AssetsBundle& bundle, std::string asset) = 0;

	virtual Result<std::unique_ptr<AudioStream>>
	createStreamFromBundle(AssetsBundle& bundle, std::string asset) = 0;

	virtual void play(AudioClip& clip) = 0;
	virtual void play(AudioStream& stream, bool loop = true) = 0;
};

} // namespace Growl
