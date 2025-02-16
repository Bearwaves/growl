#include "soloud_audio.h"

using Growl::SoLoudAudioStream;

SoLoudAudioStream::~SoLoudAudioStream() {
	stop();
}

void SoLoudAudioStream::play(bool loop) {
	if (handle) {
		api.getSoloud()->setPause(handle, false);
	} else {
		handle = api.getSoloud()->play(*stream);
		api.getSoloud()->setLooping(handle, loop);
	}
}

void SoLoudAudioStream::stop() {
	if (handle) {
		api.getSoloud()->stop(handle);
	} else {
		stream->stop();
	}
}

float SoLoudAudioStream::getVolume() {
	if (handle) {
		return api.getSoloud()->getVolume(handle);
	}
	return stream->mVolume;
}

void SoLoudAudioStream::setVolume(float volume) {
	if (handle) {
		api.getSoloud()->setVolume(handle, volume);
	} else {
		stream->setVolume(volume);
	}
}
