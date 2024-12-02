#include "soloud_audio.h"

using Growl::SoLoudAudioClip;

void SoLoudAudioClip::play() {
	if (handle) {
		api.getSoloud()->setPause(handle, false);
	} else {
		handle = api.getSoloud()->play(*sample);
	}
}

float SoLoudAudioClip::getVolume() {
	if (handle) {
		return api.getSoloud()->getVolume(handle);
	}
	return sample->mVolume;
}

void SoLoudAudioClip::setVolume(float volume) {
	if (handle) {
		api.getSoloud()->setVolume(handle, volume);
	} else {
		sample->setVolume(volume);
	}
}
