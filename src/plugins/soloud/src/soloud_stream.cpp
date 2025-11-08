#include "soloud_audio.h"

using Growl::SoLoudAudioStream;

SoLoudAudioStream::~SoLoudAudioStream() {
	stop();
}

void SoLoudAudioStream::prepare() {
	if (!api.getSoloud()->isValidVoiceHandle(handle)) {
		handle = api.getSoloud()->play(*this->stream, -1.f, 0.f, 1.f);
	}
	api.getSoloud()->seek(handle, 0.f);
}

void SoLoudAudioStream::play(bool loop) {
	if (api.getSoloud()->isValidVoiceHandle(handle)) {
		api.getSoloud()->setLooping(handle, loop);
		api.getSoloud()->setPause(handle, false);
	} else {
		handle = api.getSoloud()->play(*stream);
		api.getSoloud()->setLooping(handle, loop);
	}
}

void SoLoudAudioStream::stop() {
	if (api.getSoloud()->isValidVoiceHandle(handle)) {
		api.getSoloud()->stop(handle);
		handle = 0;
	} else {
		stream->stop();
	}
}

float SoLoudAudioStream::getVolume() {
	if (api.getSoloud()->isValidVoiceHandle(handle)) {
		return api.getSoloud()->getVolume(handle);
	}
	return stream->mVolume;
}

void SoLoudAudioStream::setVolume(float volume) {
	if (api.getSoloud()->isValidVoiceHandle(handle)) {
		api.getSoloud()->setVolume(handle, volume);
	} else {
		stream->setVolume(volume);
	}
}
