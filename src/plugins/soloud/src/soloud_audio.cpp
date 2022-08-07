#include "soloud_audio.h"
#include "growl/core/assets/audio.h"
#include "growl/core/assets/bundle.h"
#include "growl/core/assets/error.h"
#include "growl/core/audio/device.h"
#include "growl/core/error.h"
#include "soloud.h"
#include "soloud_wav.h"
#include <memory>
#include <string>
#include <vector>

using Growl::AssetsBundle;
using Growl::Error;
using Growl::Result;
using Growl::SFX;
using Growl::SoLoudAudioAPI;
using Growl::SoLoudSFX;

Error SoLoudAudioAPI::init() {
	soloud = std::make_unique<SoLoud::Soloud>();
	soloud->init();
	soloud->setGlobalVolume(1.0);

	this->devices = std::vector<AudioDevice>{
		{soloud->getBackendString(),
		 static_cast<int>(soloud->getBackendSamplerate())}};

	system.log(
		"SoLoudAudioAPI",
		"Initialised SoLoud audio; using [{}] backend at {}Hz",
		devices[0].getName(), devices[0].getSampleRate());

	return nullptr;
}

void SoLoudAudioAPI::dispose() {
	system.log("SoLoudAudioAPI", "Shutting down SoLoud");
	soloud->deinit();
}

Result<std::unique_ptr<SFX>>
SoLoudAudioAPI::loadSFXFromBundle(AssetsBundle& bundle, std::string path) {
	auto wav = std::make_unique<SoLoud::Wav>();
	auto bundle_result = bundle.getRawData(path);
	if (bundle_result.hasError()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load SFX: " + bundle_result.error()->message()));
	}
	auto raw = std::move(bundle_result.get());
	if (int error = wav->loadMem(raw.data(), raw.size(), false, false); error) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load SFX: got SoLoud error code " +
			std::to_string(error)));
	}
	std::unique_ptr<SFX> ret(
		std::make_unique<SoLoudSFX>(path, std::move(wav), std::move(raw)));
	return std::move(ret);
}

void SoLoudAudioAPI::play(SFX& sfx) {
	auto& soloud_sfx = static_cast<SoLoudSFX&>(sfx);
	soloud->play(*soloud_sfx.sample);
}
