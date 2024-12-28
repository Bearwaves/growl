#pragma once

#include "growl/core/api/api_internal.h"
#include "growl/core/assets/audio.h"
#include "growl/core/assets/bundle.h"
#include "growl/core/error.h"
#include "soloud.h"
#include "soloud_file.h"
#include "soloud_wav.h"
#include "soloud_wavstream.h"
#include <memory>
namespace Growl {

class SoLoudAudioAPI : public AudioAPIInternal {
public:
	explicit SoLoudAudioAPI(SystemAPI& system)
		: system{system} {}
	Error init() override;
	void dispose() override;

	const std::vector<AudioDevice>& getDevices() override {
		return devices;
	}

	Result<std::unique_ptr<AudioClip>>
	loadClipFromBundle(AssetsBundle& bundle, std::string name) override;

	Result<std::unique_ptr<AudioStream>>
	createStreamFromBundle(AssetsBundle& bundle, std::string name) override;

	SoLoud::Soloud* getSoloud() {
		return soloud.get();
	}

private:
	SystemAPI& system;
	std::unique_ptr<SoLoud::Soloud> soloud;
	std::vector<AudioDevice> devices;
};

class SoLoudAudioClip : public AudioClip {

public:
	SoLoudAudioClip(
		std::string name, SoLoudAudioAPI& api, std::unique_ptr<SoLoud::Wav> wav,
		std::vector<unsigned char>&& raw)
		: AudioClip(name)
		, api{api}
		, sample{std::move(wav)}
		, raw{std::move(raw)} {}

	void play(bool loop) override;
	float getVolume() override;
	void setVolume(float volume) override;

private:
	SoLoudAudioAPI& api;
	std::unique_ptr<SoLoud::Wav> sample;
	std::vector<unsigned char> raw;
	int handle = 0;
};

class SoLoudAudioStream : public AudioStream {

public:
	SoLoudAudioStream(
		std::string name, SoLoudAudioAPI& api,
		std::unique_ptr<SoLoud::WavStream> wav,
		std::unique_ptr<SoLoud::File> file)
		: AudioStream(name)
		, api{api}
		, stream{std::move(wav)}
		, file{std::move(file)} {}

	void play(bool loop) override;
	float getVolume() override;
	void setVolume(float volume) override;

private:
	SoLoudAudioAPI& api;
	std::unique_ptr<SoLoud::WavStream> stream;
	std::unique_ptr<SoLoud::File> file;
	int handle = 0;
};

}; // namespace Growl
