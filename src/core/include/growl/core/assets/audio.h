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

	virtual void play(bool loop = false) = 0;
	virtual void stop() = 0;
	virtual float getVolume() = 0;
	virtual void setVolume(float volume) = 0;

private:
	std::string name;
};

class AudioStream {
public:
	AudioStream(std::string name)
		: name{name} {}
	virtual ~AudioStream() = default;
	// SFX is move-only
	AudioStream(const AudioStream&) = delete;
	AudioStream& operator=(const AudioStream&) = delete;
	AudioStream(AudioStream&&) = default;
	AudioStream& operator=(AudioStream&&) = default;

	virtual void prepare() = 0;
	virtual void play(bool loop) = 0;
	virtual void stop() = 0;
	virtual float getVolume() = 0;
	virtual void setVolume(float volume) = 0;

private:
	std::string name;
};

} // namespace Growl
