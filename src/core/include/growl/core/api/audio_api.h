#pragma once

#include "growl/core/audio/device.h"
#include <memory>
#include <vector>
namespace Growl {

class AudioAPI {
public:
	virtual ~AudioAPI() {}

	virtual const std::vector<AudioDevice>& getDevices() = 0;
};

} // namespace Growl
