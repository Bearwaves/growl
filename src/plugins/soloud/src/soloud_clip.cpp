#include "soloud_audio.h"

using Growl::SoLoudAudioClip;

SoLoudAudioClip::~SoLoudAudioClip() {
	stop();
}

void SoLoudAudioClip::play(bool loop) {
	if (handle && loop) {
		api.getSoloud()->setPause(handle, false);
	} else {
		handle = api.getSoloud()->play(*sample);
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
