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

void SoLoudAudioStream::play(bool loop, double delay_s) {
	if (!api.getSoloud()->isValidVoiceHandle(handle)) {
		handle = api.getSoloud()->play(*this->stream, -1.f, 0.f, 1.f);
	}
	api.getSoloud()->setLooping(handle, loop);
	api.getSoloud()->setDelaySamples(
		handle, std::round(delay_s * api.getSoloud()->getSamplerate(handle)));
	play_delay = delay_s;
	api.getSoloud()->setPause(handle, false);
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

void SoLoudAudioStream::setLoopPoint(double time_s) {
	if (api.getSoloud()->isValidVoiceHandle(handle)) {
		api.getSoloud()->setLoopPoint(handle, time_s);
	} else {
		stream->setLoopPoint(time_s);
	}
}

double SoLoudAudioStream::getPosition() {
	if (!api.getSoloud()->isValidVoiceHandle(handle)) {
		return 0.0;
	}
	double pos = api.getSoloud()->getStreamPosition(handle);
	if (play_delay && !getLoopCount()) {
		return pos - play_delay;
	}
	return pos;
}

double SoLoudAudioStream::getLength() {
	return stream->getLength();
}

double SoLoudAudioStream::getPlaytime() {
	if (!api.getSoloud()->isValidVoiceHandle(handle)) {
		return 0.0;
	}
	return std::max(0.0, api.getSoloud()->getStreamTime(handle) - play_delay);
}

int SoLoudAudioStream::getLoopCount() {
	if (!api.getSoloud()->isValidVoiceHandle(handle)) {
		return 0;
	}
	return api.getSoloud()->getLoopCount(handle);
}
