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

class SoLoudAudioClip : public AudioClip {
	friend class SoLoudAudioAPI;

public:
	SoLoudAudioClip(
		std::string name, std::unique_ptr<SoLoud::Wav> wav,
		std::vector<unsigned char>&& raw)
		: AudioClip(name)
		, sample{std::move(wav)}
		, raw{std::move(raw)} {}

private:
	std::unique_ptr<SoLoud::Wav> sample;
	std::vector<unsigned char> raw;
};

class SoLoudAudioStream : public AudioStream {
	friend class SoLoudAudioAPI;

public:
	SoLoudAudioStream(
		std::string name, std::unique_ptr<SoLoud::WavStream> wav,
		std::unique_ptr<SoLoud::File> file)
		: AudioStream(name)
		, stream{std::move(wav)}
		, file{std::move(file)} {}

private:
	std::unique_ptr<SoLoud::WavStream> stream;
	std::unique_ptr<SoLoud::File> file;
};

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

	void play(AudioClip& sfx) override;
	void play(AudioStream& stream, bool loop = true) override;

private:
	SystemAPI& system;
	std::unique_ptr<SoLoud::Soloud> soloud;
	std::vector<AudioDevice> devices;
};

}; // namespace Growl
