#pragma once

#include "SDL3/SDL_asyncio.h"
#include "growl/core/system/preferences.h"
#include <unordered_map>

namespace Growl {

class SDL3SystemAPI;

class SDL3Preferences final : public Preferences {
public:
	SDL3Preferences(
		SDL3SystemAPI& api, std::filesystem::path prefs_file, bool shared,
		nlohmann::json&& j);
	~SDL3Preferences();
	void store() override;

private:
	SDL3SystemAPI& api;
	std::filesystem::path prefs_file;
	SDL_AsyncIOQueue* queue;
	SDL_AsyncIO* file;
	uint64_t writes = 0;
	std::unordered_map<uint64_t, std::string> strings;

	void processResults();
};

} // namespace Growl
