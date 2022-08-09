#pragma once

#include <string>
namespace Growl {

class AudioClip {
public:
	AudioClip(std::string name)
		: name{name} {}
	virtual ~AudioClip() = default;
	// SFX is move-only
	AudioClip(const AudioClip&) = delete;
	AudioClip& operator=(const AudioClip&) = delete;
	AudioClip(AudioClip&&) = default;
	AudioClip& operator=(AudioClip&&) = default;

private:
	std::string name;
};

bool isValidAudio(std::string path);

} // namespace Growl
