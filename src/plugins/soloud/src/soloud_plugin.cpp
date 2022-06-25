#include "growl/core/api/api.h"
#include "soloud_audio.h"

using Growl::API;
using Growl::SoLoudAudioAPI;

void initSoLoudPlugin(API& api) {
	api.addAudioAPI(std::make_unique<SoLoudAudioAPI>(api.system()));
}
