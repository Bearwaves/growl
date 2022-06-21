#pragma once

#include <string>
namespace Growl {

class AudioDevice {
public:
	AudioDevice(std::string name, int sample_rate)
		: name{name}
		, sample_rate{sample_rate} {}
	const std::string getName() const {
		return name;
	}
	const int getSampleRate() const {
		return sample_rate;
	}

private:
	std::string name;
	int sample_rate;
};

} // namespace Growl
