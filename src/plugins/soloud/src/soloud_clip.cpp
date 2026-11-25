#include "soloud_audio.h"

using Growl::SoLoudAudioClip;

SoLoudAudioClip::~SoLoudAudioClip() {
	stop();
}

void SoLoudAudioClip::play(bool loop, float pan, float playback_speed) {
	if (handle && loop) {
		api.getSoloud()->setPause(handle, false);
	} else {
		handle = api.getSoloud()->play(*sample);
		if (playback_speed != 1.f) {
			api.getSoloud()->setRelativePlaySpeed(handle, playback_speed);
		}
		if (pan) {
			api.getSoloud()->setPan(handle, pan);
		}
		api.getSoloud()->setLooping(handle, loop);
	}
}

void SoLoudAudioClip::stop() {
	if (handle) {
		api.getSoloud()->stop(handle);
	} else {
		sample->stop();
	}
}

float SoLoudAudioClip::getVolume() {
	return sample->mVolume;
}

void SoLoudAudioClip::setVolume(float volume) {
	if (handle) {
		api.getSoloud()->setVolume(handle, volume);
	}
	sample->setVolume(volume);
}
