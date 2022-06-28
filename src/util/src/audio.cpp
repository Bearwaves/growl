#include "growl/util/assets/audio.h"
#include "soloud_wavstream.h"

bool Growl::isValidAudio(std::string path) {
	SoLoud::WavStream sound;
	return sound.load(path.c_str()) == SoLoud::SO_NO_ERROR;
}
