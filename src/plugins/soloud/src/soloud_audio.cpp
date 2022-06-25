#include "soloud_audio.h"
#include "growl/core/audio/device.h"
#include "growl/util/error.h"
#include "soloud.h"
#include <memory>
#include <vector>

using Growl::Error;
using Growl::SoLoudAudioAPI;

Error SoLoudAudioAPI::init() {
	soloud = std::make_unique<SoLoud::Soloud>();
	soloud->init();

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
