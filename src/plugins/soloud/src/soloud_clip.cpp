#include "soloud_audio.h"

using Growl::SoLoudAudioClip;

void SoLoudAudioClip::play(bool loop) {
	if (handle && loop) {
		api.getSoloud()->setPause(handle, false);
	} else {
		handle = api.getSoloud()->play(*sample);
		api.getSoloud()->setLooping(handle, loop);
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
