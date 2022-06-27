#include "growl/util/assets/sound.h"
#include "soloud_wavstream.h"

bool Growl::isValidSound(std::string path) {
	SoLoud::WavStream sound;
	return sound.load(path.c_str()) == SoLoud::SO_NO_ERROR;
}
